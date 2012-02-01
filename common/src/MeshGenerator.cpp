#include "MeshGenerator.h"

#include <nvmath/nvmath.h>
#include <nvsg/ViewState.h>
#include <nvsg/Face.h>
#include <nvsg/FaceAttribute.h>
#include <nvsg/GeoNode.h>
#include <nvsg/LOD.h>
#include <nvsg/Material.h>
#include <nvsg/Node.h>
#include <nvsg/PointLight.h>
#include <nvsg/SpotLight.h>
#include <nvsg/StateSet.h>
#include <nvsg/Switch.h>
#include <nvsg/TextureAttribute.h>
#include <nvsg/Transform.h>
#include <nvsg/TriPatches4.h>
#include <nvsg/QuadPatches4x4.h>
#include <nvutil/Tools.h>
#include <nvutil/PlugIn.h>
#include <nvsg/PlugInterface.h>
#include <nvsg/PlugInterfaceID.h>

#include "nvsg/DirectedLight.h"

#include <vector>

#include "nvutil/DbgNew.h" // this must be the last include

using namespace nvmath;
using namespace nvsg;
using namespace nvutil;
using namespace std;

namespace nvutil
{
namespace
{
//! Helper function to calculate the face normal of the face a, b, c
Vec3f calculateFaceNormal( const Vec3f &a, const Vec3f &b, const Vec3f &c )
{
    Vec3f d1 = b - a;
    Vec3f d2 = c - a;
    Vec3f n  = d1 ^ d2;
    n.normalize();
    return n;
}

StateSetSharedPtr createPatchesStateSet( const std::string & tessFile, const std::vector<std::string> & searchPaths )
{
    // Create the tesselation shader first
    CgFxSharedPtr tessCgFx = CgFx::create();
    {
        CgFxEffectWriteLock effect( CgFxReadLock( tessCgFx )->getEffect() );
        std::string file;
        std::string err;
        if (   !FindFileFirst( tessFile, searchPaths, file )
               || !effect->createFromFile( file, searchPaths, err ) )
        {
            return( StateSetSharedPtr() );
        }
    }

    // Create a Material
    MaterialSharedPtr material = Material::create();
    {
        MaterialWriteLock m(material);
        m->setAmbientColor(  Vec3f(0.0f, 0.0f, 0.0f) );
        m->setDiffuseColor(  Vec3f(0.0f, 0.0f, 1.0f) );
        m->setEmissiveColor( Vec3f(0.0f, 0.0f, 0.0f) );
        m->setSpecularColor( Vec3f(0.3f, 0.3f, 0.3f) );
        m->setSpecularExponent( 10.f );
        m->setOpacity( 1.0f );
    }

    // Create a StateSet and attach material and shader
    StateSetSharedPtr stateSet = StateSet::create();
    {
        StateSetWriteLock ss( stateSet );
        ss->addAttribute( material );
        ss->addAttribute( tessCgFx );
    }

    return( stateSet );
}

//! Helper function to setup the faces, vertices, normals and texccords of a tessellated plane
//with \a subdiv subdivisions and a transformation-matrix transf
void setupTessellatedPlane( unsigned int subdiv, const Mat44f &transf,
                            vector < Face3 > &faces, vector < Vec3f > &vertices,
                            vector < Vec3f > &normals, vector < Vec2f > &texcoords,
                            vector <unsigned int> &indices )
{
    float step = 2.0f/(float)(subdiv + 1);
    unsigned int row = subdiv + 2;
    unsigned int offset = checked_cast<unsigned int>(vertices.size());


    // This is expensive do it once outside the loops!
    Mat44f transfIT;
    bool validInverse = invert( transf, transfIT );
    if (validInverse)
    {
        transfIT = ~transfIT;
    }

    float y = -1.0f;
    for ( unsigned int sY = 0; sY < row; sY++ )
    {
        float x = -1.0f;
        for ( unsigned int sX = 0; sX < row; sX++ )
        {
            vertices.push_back( Vec3f(Vec4f( x, y, 0.0f, 1.0f ) * transf ) );
            Vec3f normal(0.0f, 0.0f, 1.0f); // Initialize to some valid normal in case the transf matrix cannot be inverted.
            if ( validInverse )
            {
                normal = Vec3f(Vec4f( 0.0f, 0.0f, 1.0f, 0.0f) * transfIT);
                normal.normalize();
            }
            normals.push_back( normal );
            texcoords.push_back( Vec2f( x * 0.5f + 0.5f, y * 0.5f + 0.5f ) );
            x += step;
        }
        y += step;
    }

    for ( unsigned int sY = 0; sY <= subdiv; sY++ )
    {
        for ( unsigned int sX = 0; sX <= subdiv; sX++ )
        {
            indices.push_back( offset + sX + sY * row );
            indices.push_back( offset + sX + 1 + sY * row );
            indices.push_back( offset + sX + 1 + (sY+1) * row );

            indices.push_back( offset + sX + 1 + (sY+1) * row );
            indices.push_back( offset + sX + (sY+1) * row );
            indices.push_back( offset + sX + sY * row );

            Face3 f;

            f[0] = offset + sX + sY * row;
            f[1] = offset + sX + 1 + sY * row;
            f[2] = offset + sX + 1 + (sY+1) * row;
            faces.push_back( f );

            f[0] = offset + sX + 1 + (sY+1) * row;
            f[1] = offset + sX + (sY+1) * row;
            f[2] = offset + sX + sY * row;
            faces.push_back( f );
        }
    }
}
}

// ===========================================================================

DrawableSharedPtr createQuadSet( unsigned int m, unsigned int n, const float size, const float gap )
{
    NVSG_ASSERT( m >= 1 && n >= 1 && "createQuadSet(): m and n both have to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals and faces for n X m tiles
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;
    vector<Face4> faces;

    const int size_v = 4 * m * n;
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( m * n );

    // lower-left corner of the current tile
    float dy = 0.0f;

    // n tiles in x-direction
    for( unsigned int i = 0; i < m; ++i )
    {
        // reset x-offset
        float dx = 0.0f;

        // n tiles in x-direction
        for( unsigned int j = 0; j < n; ++j )
        {
            // add 4 vertices
            Vec3f a = Vec3f( dx       , dy       ,              0.0f );
            Vec3f b = Vec3f( dx + size, dy       , (float)j/(float)n );
            Vec3f c = Vec3f( dx + size, dy + size, (float)j/(float)n );
            Vec3f d = Vec3f( dx       , dy + size,              0.0f );
            vertices.push_back( a );
            vertices.push_back( b );
            vertices.push_back( c );
            vertices.push_back( d );

            unsigned int first_index = ( i * n + j ) * 4;
            Vec3f n = calculateFaceNormal( a, b, d );

            // Setup normals and faces
            for( unsigned int k = 0; k < 4; ++k )
            {
                normals.push_back(n);
                indices.push_back( first_index + k );
            }

            dx += size + gap;
        }
        dy += size + gap;
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_QUADS );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createQuadStrip( unsigned int n, float height, float radius )
{
    NVSG_ASSERT( n >= 1 && "createQuadStrip(): n has to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals and indices
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;

    const int size_v = 2*(n+1);
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );

    // for n quads, we need 2*(n+1) vertices, so run to including n
    for( unsigned int i = 0; i <= n; ++i )
    {
        float phi = PI * ( 1.0f - (float)i/(float)n );
        float x = cos(phi);
        float z = sin(phi);
        Vec3f v( x, 0.0f, z );

        normals.push_back( v );
        normals.push_back( v );
        vertices.push_back( v * radius + Vec3f( 0.0f, height, 0.0f ) );
        vertices.push_back( v * radius );

        indices.push_back( i * 2 );
        indices.push_back( i * 2 + 1 );

    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas(vasPtr);
        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_QUAD_STRIP );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createTriSet( unsigned int m, unsigned int n, const float size, const float gap )
{
    NVSG_ASSERT( m >= 1 && n >= 1 && "createTriSet(): m and n both have to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals and faces for n X m tiles
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;
    vector<Face3> faces;

    const int size_v = 3 * m * n;
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( m * n );

    // lower-left corner of the current tile
    float dy = 0.0f;

    // m tiles in y-direction
    for( unsigned int i = 0; i < m; ++i )
    {
        // reset x-offset
        float dx = 0.0f;

        // n tiles in x-direction
        for( unsigned int j = 0; j < n; ++j )
        {
            // add 3 vertices
            Vec3f a = Vec3f( dx       , dy       ,              0.0f );
            Vec3f b = Vec3f( dx + size, dy       , (float)j/(float)n );
            Vec3f c = Vec3f( dx       , dy + size,              0.0f );
            vertices.push_back( a );
            vertices.push_back( b );
            vertices.push_back( c );

            // Setup faces and normals
            unsigned int first_index = ( i * n + j ) * 3;
            Vec3f n = calculateFaceNormal( a, b, c );

            for( unsigned int k = 0; k < 3; ++k )
            {
                normals.push_back(n);
                indices.push_back( first_index + k );
            }

            dx += size + gap;
        }
        dy += size + gap;
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas(vasPtr);
        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createTriFan( unsigned int n, const float radius, const float elevation )
{
    NVSG_ASSERT( n > 1 && "createTriFan(): n has to be at least 2." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices and faces for n triangles
    vector<Vec3f> vertices;
    vector<unsigned int> indices;
    
    vertices.reserve( n + 2 );
    indices.reserve( n + 2 );

    vertices.push_back( Vec3f( 0.0f, 0.0f, elevation ) );
    vertices.push_back( Vec3f( radius, 0.0f, 0.0f ) );

    indices.push_back(0);
    indices.push_back(1);

    // n triangles ( i from 1 to n ! )
    for( unsigned int i = 1; i <= n; ++i )
    {
        float phi = PI * ((float)i/(float)n);
        float x = radius * cos(phi);
        float y = radius * sin(phi);
        vertices.push_back( Vec3f(x, y, 0.f) );
        indices.push_back( i + 1 );
    }

    // Calculate normals
    vector<Vec3f> normals;
    normals.resize(vertices.size());
    for( size_t i = 0; i < normals.size(); ++i )
    {
        normals[i] = Vec3f( 0.0f, 0.0f, 0.0f );
    }

    for( unsigned int i = 0; i < n; ++i )
    {
        // calculate face normal for face i
        Vec3f fn = calculateFaceNormal( vertices.at(0), vertices.at(i+1), vertices.at(i+2) );
        // accumulate in vertex normals
        normals.at(0)   += fn;
        normals.at(i+1) += fn;
        normals.at(i+2) += fn;
    }

    for( size_t i = 0; i < normals.size(); ++i )
    {
        normals[i].normalize();
    }


    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas(vasPtr);
        vas->setVertices( &vertices[0], n + 2 );
        vas->setNormals( &normals[0], n + 2 );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLE_FAN );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createTriStrip( unsigned int rows, unsigned int columns, float width, float height )
{
    NVSG_ASSERT( rows >= 1 && columns >= 1 && "createTriStrip(): rows and columns both have to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // Setup vertices, normals and indices
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;
    std::vector< IndexList > indexlist;

    unsigned int size_v = ( rows + 1 ) * ( columns + 1 );
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );
    
    for( unsigned int i = 0; i <= rows; ++i )
    {
        for( unsigned int j = 0; j <= columns; ++j )
        {
            vertices.push_back( Vec3f( j * width, i * height, 0.0f ) );
            normals.push_back( Vec3f( 0.0f, 0.0f, 1.0f ) );
        }
    }

    for( unsigned int i = 0; i < rows; ++i )
    {
        for( unsigned int j = 0; j <= columns; ++j )
        {
            indices.push_back( i * ( columns + 1 ) + j + columns + 1 );
            indices.push_back( i * ( columns + 1 ) + j );
        }
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLE_STRIP );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    /*
    FaceAttributeSharedPtr faceHdl = FaceAttribute::create();
    {
      FaceAttributeWriteLock(faceHdl)->setTwoSidedLighting( true );
      FaceAttributeWriteLock(faceHdl)->setCullMode( false, false );
    }
    */

    return drawablePtr;
}

// ===========================================================================

GeoNodeSharedPtr createTriPatches4( const std::vector<std::string> & searchPaths, unsigned int m, unsigned int n, const Vec3f & size, const Vec2f & offset )
{
    // Create a StateSet with a Material and a CgFx first
    StateSetSharedPtr stateSet = createPatchesStateSet( "phongTessPatch10V.fx", searchPaths );
    if ( ! stateSet )
    {
        return( GeoNodeSharedPtr() );
    }

    // Set up the tri patch:
    //  9
    //  7 8
    //  4 5 6
    //  0 1 2 3
    //
    Vec3f verts[10] =
    {
        Vec3f(size[0]*0.0f/4.0f, size[1]*0.0f/4.0f, size[2]*0.0f/4.0f), // 0
        Vec3f(size[0]*1.0f/4.0f, size[1]*0.0f/4.0f, size[2]*1.0f/4.0f), // 1
        Vec3f(size[0]*2.0f/4.0f, size[1]*0.0f/4.0f, size[2]*1.0f/4.0f), // 2
        Vec3f(size[0]*3.0f/4.0f, size[1]*0.0f/4.0f, size[2]*0.0f/4.0f), // 3
        Vec3f(size[0]*0.0f/4.0f, size[1]*1.0f/4.0f, size[2]*1.0f/4.0f), // 4
        Vec3f(size[0]*1.0f/4.0f, size[1]*1.0f/4.0f, size[2]*2.0f/4.0f), // 5
        Vec3f(size[0]*2.0f/4.0f, size[1]*1.0f/4.0f, size[2]*1.0f/4.0f), // 6
        Vec3f(size[0]*0.0f/4.0f, size[1]*2.0f/4.0f, size[2]*1.0f/4.0f), // 7
        Vec3f(size[0]*1.0f/4.0f, size[1]*2.0f/4.0f, size[2]*1.0f/4.0f), // 8
        Vec3f(size[0]*0.0f/4.0f, size[1]*3.0f/4.0f, size[2]*0.0f/4.0f)  // 9
    };

    std::vector<Vec3f> vertices;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    for( unsigned int i = 0; i < m; ++i )
    {
        for( unsigned int j = 0; j < n; ++j )
        {
            for( unsigned int k = 0; k < 10; ++k )
            {
                vertices.push_back( verts[k] + Vec3f(x, y, z) );
            }
            x += offset[0];
        }
        y += offset[1];
        x = 0.0f;
    }

    // Create a VertexAttributeSet
    VertexAttributeSetSharedPtr vas = VertexAttributeSet::create();
    VertexAttributeSetWriteLock( vas )->setVertices( &vertices[0], checked_cast<unsigned int>(vertices.size()) );

    // Create a TriPatches4
    TriPatches4SharedPtr triPatches = TriPatches4::create();
    TriPatches4WriteLock( triPatches )->setVertexAttributeSet( vas );

    // Create a GeoNode and add the geometry
    GeoNodeSharedPtr geoNode = GeoNode::create();
    GeoNodeWriteLock( geoNode )->addDrawable( stateSet, triPatches );

    return geoNode;
}

// ===========================================================================

GeoNodeSharedPtr createQuadPatches4x4( const std::vector<std::string> & searchPaths, unsigned int n, unsigned int m, const Vec2f & offset )
{
    // Create a StateSet with a Material and a CgFx
    StateSetSharedPtr stateSet = createPatchesStateSet( "phongTessPatch16V.fx", searchPaths );
    if ( ! stateSet )
    {
        return( GeoNodeSharedPtr() );
    }

    const float r = std::min( offset[0], offset[1] )/2.0f * 0.75f;
    const float dy = r;  // distance between rows of vertices

    // For a cylinder, we need four quad patches, each extruding an approximation
    // of a quarter of a circle

    // For a bezier curve to approximate a quarter of a circle, we need to use a
    // cubic bezier curve with inner points at a distance x of r*4(sqrt(2)-1)/3 from
    // the end points in the tangent direction at the end points:
    //
    //   0--d--1
    //   |      \
    //   r       2
    //   |       |
    //   *       3
    //
    
    // Distance of the boundary control points to the corner control points
    const float d = r * 4.0f * (sqrt(2.0f)-1.0f)/3.0f;

    // A zero for nice alignment
    const float o = 0.0f;
    
    // First construct the bezier curves to be extruded
    Vec3f bezierCurves[16];
    bezierCurves[ 0] = Vec3f( o, o, r ); //
    bezierCurves[ 1] = Vec3f( d, o, r ); //
    bezierCurves[ 2] = Vec3f( r, o, d ); //
    bezierCurves[ 3] = Vec3f( r, o, o ); //        |
    bezierCurves[ 4] = Vec3f( r, o, o ); //        |
    bezierCurves[ 5] = Vec3f( r, o,-d ); //      9 786
    bezierCurves[ 6] = Vec3f( d, o,-r ); //     A  |  5
    bezierCurves[ 7] = Vec3f( o, o,-r ); //  --BC-----34--x
    bezierCurves[ 8] = Vec3f( o, o,-r ); //     D  |  2
    bezierCurves[ 9] = Vec3f(-d, o,-r ); //      E F0 1
    bezierCurves[10] = Vec3f(-r, o,-d ); //        |
    bezierCurves[11] = Vec3f(-r, o, o ); //        z
    bezierCurves[12] = Vec3f(-r, o, o ); //
    bezierCurves[13] = Vec3f(-r, o, d ); //
    bezierCurves[14] = Vec3f(-d, o, r ); //
    bezierCurves[15] = Vec3f( o, o, r ); //

    // Extrude the curves to form the patches
    //            y  C D E F
    // one patch: |  8 9 A B
    //            |  4 5 6 7
    //            O  0 1 2 3
    vector< Vec3f > verts;
    for( unsigned int i = 0; i < 4; ++i )
    {
        // patch i
        for( unsigned int j = 0; j < 4; ++j )
        {
            // row j
            for( unsigned int k = 0; k < 4; ++k )
            {
                // column k
                verts.push_back( bezierCurves[i*4+k] + Vec3f( o, dy*j, o ));
            }
        }
    }

    // Generate n x m cylinders
    vector< Vec3f > vertices;
    for( unsigned int i = 0; i < n; ++i )
    {
        for( unsigned int j = 0; j < m; ++j )
        {
            float x = i * offset[0];
            float z = j * offset[1];
            for( unsigned int k = 0; k < verts.size(); ++k )
            {
                vertices.push_back( verts.at( k ) + Vec3f( x, o, z ) );
            }
        }
    }

    // Create a VertexAttributeSet
    VertexAttributeSetSharedPtr vas = VertexAttributeSet::create();
    VertexAttributeSetWriteLock( vas )->setVertices( &vertices[0], checked_cast<unsigned int>( vertices.size() ) );

    // Create a PrimitiveSet
    QuadPatches4x4SharedPtr patches = QuadPatches4x4::create();
    QuadPatches4x4WriteLock( patches )->setVertexAttributeSet( vas );

    // Create a GeoNode and add the geometry
    GeoNodeSharedPtr geoNode = GeoNode::create();
    GeoNodeWriteLock( geoNode )->addDrawable( stateSet, patches );

    return geoNode;
}

// ===========================================================================

DrawableSharedPtr createCube()
{
    // Right handed model coordinates.
    // Vertex numbering and coordinate setup match the 3-bit pattern: (z << 2) | (y << 1) | x
    // ASCII art:
    /*
         y

         2--------------3
        /              /|
       / |            / |
      6--------------7  |
      |  |           |  |
      |              |  |
      |  |           |  |
      |  0 -  -  -  -| -1  x
      | /            | /
      |/             |/
      4--------------5
     z
    */
    
    // Setup vertices
    static const Vec3f vertices[8] =
    {
        Vec3f( -1.0f, -1.0f, -1.0f ), // 0
        Vec3f(  1.0f, -1.0f, -1.0f ), // 1
        Vec3f( -1.0f,  1.0f, -1.0f ), // 2
        Vec3f(  1.0f,  1.0f, -1.0f ), // 3
        Vec3f( -1.0f, -1.0f,  1.0f ), // 4
        Vec3f(  1.0f, -1.0f,  1.0f ), // 5
        Vec3f( -1.0f,  1.0f,  1.0f ), // 6
        Vec3f(  1.0f,  1.0f,  1.0f )  // 7
    };

    // Setup faces
    static const Face3 faces[12] =
    {
        {1, 0, 2}, {2, 3, 1}, // back
        {0, 4, 6}, {6, 2, 0}, // left
        {0, 1, 5}, {5, 4, 0}, // bottom
        {4, 5, 7}, {7, 6, 4}, // front
        {5, 1, 3}, {3, 7, 5}, // right
        {3, 2, 6}, {6, 7, 3}  // top
    };

    // Setup texture coordinates
    static const Vec2f texcoords[4] =
    {
        Vec2f(0.0f, 0.0f),
        Vec2f(1.0f, 0.0f),
        Vec2f(1.0f, 1.0f),
        Vec2f(0.0f, 1.0f)
    };

    Vec3f v[36];
    Vec3f n[36];
    Vec2f tc[36];
    vector<unsigned int> indices;
    
    Vec3f v0, v1, v2, fn;

    for ( int kf = 0, kv = 0; kf < 12; kf++, kv += 3 )
    {
        v0 = vertices[faces[kf][0]];
        v1 = vertices[faces[kf][1]];
        v2 = vertices[faces[kf][2]];
        fn = calculateFaceNormal( v0, v1, v2 );

        indices.push_back(kv);
        indices.push_back(kv+1);
        indices.push_back(kv+2);

        v[kv]    = v0;
        v[kv+1]  = v1;
        v[kv+2]  = v2;

        n[kv]    = fn;
        n[kv+1]  = fn;
        n[kv+2]  = fn;

        // Assign texture coordinates
        if (kf & 1)
        { // odd faces
            tc[kv]    = texcoords[2];
            tc[kv+1]  = texcoords[3];
            tc[kv+2]  = texcoords[0];
        }
        else
        { // Even faces
            tc[kv]    = texcoords[0];
            tc[kv+1]  = texcoords[1];
            tc[kv+2]  = texcoords[2];
        }
    }

    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( v, 36 );
        vas->setNormals( n, 36 );
        vas->setTexCoords( 0, tc, 36 );
    }
    
    // Create a PrimitiveSet
    IndexSetSharedPtr indexSet( IndexSet::create() );
    IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
    }
    
    return primitivePtr;
}

// ===========================================================================

DrawableSharedPtr createTetrahedron()
{
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices
    static const Vec3f vertices[4] =
    {
        Vec3f( -1.0f, -1.0f, -1.0f ),
        Vec3f( 1.0f, 1.0f, -1.0f ),
        Vec3f( 1.0f, -1.0f, 1.0f ),
        Vec3f( -1.0f, 1.0f, 1.0f )
    };

    // Setup faces:
    static const Face3 faces[4] =
    {
        {0, 3, 1},
        {0, 1, 2},
        {0, 2, 3},
        {1, 3, 2}
    };

    // Setup texture coordinates
    static const Vec2f texcoords[4] =
    {
        Vec2f(0.0f, 0.0f),
        Vec2f(1.0f, 1.0f),
        Vec2f(1.0f, 0.0f),
        Vec2f(0.0f, 1.0f)
    };

    // Calculate normals
    Vec3f v[12];
    Vec3f n[12];
    Vec2f tc[12];

    Vec3f v0, v1, v2, fn;

    vector<unsigned int> indices;

    for ( int kf = 0, kv = 0; kf < 4; kf++, kv += 3 )
    {
        v0 = vertices[faces[kf][0]];
        v1 = vertices[faces[kf][1]];
        v2 = vertices[faces[kf][2]];

        fn = calculateFaceNormal( v0, v1, v2 );

        indices.push_back(kv);
        indices.push_back(kv+1);
        indices.push_back(kv+2);

        v[kv]    = v0;
        v[kv+1]  = v1;
        v[kv+2]  = v2;

        n[kv]    = fn;
        n[kv+1]  = fn;
        n[kv+2]  = fn;
    }

    // Assign texture coordinates
    tc[0]  = texcoords[0];
    tc[1]  = texcoords[1];
    tc[2]  = texcoords[2];
    tc[3]  = texcoords[0];
    tc[4]  = texcoords[3];
    tc[5]  = texcoords[1];
    tc[6]  = texcoords[0];
    tc[7]  = texcoords[2];
    tc[8]  = texcoords[3];
    tc[9]  = texcoords[1];
    tc[10] = texcoords[3];
    tc[11] = texcoords[2];


    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( v, 12 );
        vas->setNormals( n, 12 );
        vas->setTexCoords( 0, tc, 12 );
    }

    {
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createOctahedron()
{
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices
    static const Vec3f vertices[6] =
    {
        Vec3f( 0.0f, 1.0f, 0.0f ),
        Vec3f( 0.0f, -1.0f, 0.0f ),
        Vec3f( 1.0f, 0.0f, 0.0f ),
        Vec3f( -1.0f, 0.0f, 0.0f ),
        Vec3f( 0.0f, 0.0f, 1.0f ),
        Vec3f( 0.0f, 0.0f, -1.0f )
    };

    // Setup faces
    static const Face3 faces[8] =
    {
        {0, 4, 2},
        {0, 2, 5},
        {0, 5, 3},
        {0, 3, 4},
        {1, 2, 4},
        {1, 5, 2},
        {1, 3, 5},
        {1, 4, 3}
    };

    // Setup texture coordinates
    static const Vec2f texcoords[5] =
    {
        Vec2f(0.0f, 0.0f),  //0
        Vec2f(1.0f, 0.0f),  //1
        Vec2f(1.0f, 1.0f),  //2
        Vec2f(0.0f, 1.0f),  //3
        Vec2f(0.5f, 0.5f)   //4
    };

    // Calculate normals
    Vec3f v[24];
    Vec3f n[24];
    Vec2f tc[24];

    Vec3f v0, v1, v2, fn;

    vector<unsigned int> indices;

    for ( int kf = 0, kv = 0; kf < 8; kf++, kv += 3 )
    {
        v0 = vertices[faces[kf][0]];
        v1 = vertices[faces[kf][1]];
        v2 = vertices[faces[kf][2]];

        fn = calculateFaceNormal( v0, v1, v2 );

        indices.push_back(kv);
        indices.push_back(kv+1);
        indices.push_back(kv+2);

        v[kv]    = v0;
        v[kv+1]  = v1;
        v[kv+2]  = v2;

        n[kv]    = fn;
        n[kv+1]  = fn;
        n[kv+2]  = fn;
    }


    // Assign texture coordinates
    tc[0]  = texcoords[2];
    tc[1]  = texcoords[4];
    tc[2]  = texcoords[1];
    tc[3]  = texcoords[2];
    tc[4]  = texcoords[1];
    tc[5]  = texcoords[4];
    tc[6]  = texcoords[2];
    tc[7]  = texcoords[4];
    tc[8]  = texcoords[3];
    tc[9]  = texcoords[2];
    tc[10] = texcoords[3];
    tc[11] = texcoords[4];
    tc[12] = texcoords[0];
    tc[13] = texcoords[1];
    tc[14] = texcoords[4];
    tc[15] = texcoords[0];
    tc[16] = texcoords[4];
    tc[17] = texcoords[1];
    tc[18] = texcoords[0];
    tc[19] = texcoords[3];
    tc[20] = texcoords[4];
    tc[21] = texcoords[0];
    tc[22] = texcoords[4];
    tc[23] = texcoords[3];


    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( v, 24 );
        vas->setNormals( n, 24 );
        vas->setTexCoords( 0, tc, 24 );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createDodecahedron()
{
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    //USE ICOSAHEDRON VERTICES AND FACES TO SETUP DODECAHEDRON
    //--------------------------------------------------------
    static const float x = 0.525731112119133606f;
    static const float z = 0.850650808352039932f;

    static const Vec3f icoVertices[12] =
    {
        Vec3f(   -x , 0.0f ,    z ),
        Vec3f(    x , 0.0f ,    z ),
        Vec3f(   -x , 0.0f ,   -z ),
        Vec3f(    x , 0.0f ,   -z ),
        Vec3f( 0.0f ,    z ,    x ),
        Vec3f( 0.0f ,    z ,   -x ),
        Vec3f( 0.0f ,   -z ,    x ),
        Vec3f( 0.0f ,   -z ,   -x ),
        Vec3f(    z ,    x , 0.0f ),
        Vec3f(   -z ,    x , 0.0f ),
        Vec3f(    z ,   -x , 0.0f ),
        Vec3f(   -z ,   -x , 0.0f )
    };

    static const Face3 icoFaces[20] =
    {
        { 0,  1,  4}, //  0
        { 0,  4,  9}, //  1
        { 0,  9, 11}, //  2
        { 0,  6,  1}, //  3
        { 0, 11,  6}, //  4
        { 1,  6, 10}, //  5
        { 1, 10,  8}, //  6
        { 1,  8,  4}, //  7
        { 2,  3,  7}, //  8
        { 2,  5,  3}, //  9
        { 2,  9,  5}, // 10
        { 2, 11,  9}, // 11
        { 2,  7, 11}, // 12
        { 3,  5,  8}, // 13
        { 3,  8, 10}, // 14
        { 3, 10,  7}, // 15
        { 4,  5,  9}, // 16
        { 4,  8,  5}, // 17
        { 6,  7, 10}, // 18
        { 6, 11,  7}  // 19
    };

    //--------------------------------------------------------

    // Setup indices of dodecahedron
    static const int idxDode[12][5] =
    {
        { 0,  1,  2,  4,  3}, //  0
        { 0,  3,  5,  6,  7}, //  1
        { 9,  8, 12, 11, 10}, //  2
        { 9, 13, 14, 15,  8}, //  3
        { 0,  7, 17, 16,  1}, //  4
        { 9, 10, 16, 17, 13}, //  5
        {18,  5,  3,  4, 19}, //  6
        {15, 18, 19, 12,  8}, //  7
        { 6, 14, 13, 17,  7}, //  8
        { 1, 16, 10, 11,  2}, //  9
        { 5, 18, 15, 14,  6}, // 10
        { 2, 11, 12, 19,  4}  // 11
    };

    // Calculate vertices
    Vec3f vertices[20];
    Vec3f v;

    // The 20 vertices of the dodecahedron are the centers of the 20 icosahedron triangle faces
    // pushed out to unit sphere radius by normalization
    for ( int i = 0; i < 20; i++ )
    {
        v = icoVertices[icoFaces[i][0]] +  icoVertices[icoFaces[i][1]] +  icoVertices[icoFaces[i][2]];
        v.normalize();
        vertices[i] = v;
    }

    // Setup vertices, normals and faces
    vector<Vec3f> vv;
    vector<Vec3f> vn;

    vv.reserve( 72 );
    vn.reserve( 72 );

    vector<unsigned int> indices;
    vector<Face3> faces;
    indices.reserve( 180 );
    faces.reserve( 60 );
    
    for( unsigned int i = 0 ; i < 12; ++i )
    {
        Vec3f v( 0.0f, 0.0f, 0.0f );
        for( unsigned int j = 0 ; j < 5; ++j )
        {
            v += vertices[idxDode[i][j]];
        }
        v /= 5.0f;

        // center point of face
        vv.push_back(v);
        vv.push_back(vertices[idxDode[i][0]]);
        vv.push_back(vertices[idxDode[i][1]]);
        vv.push_back(vertices[idxDode[i][2]]);
        vv.push_back(vertices[idxDode[i][3]]);
        vv.push_back(vertices[idxDode[i][4]]);

        v.normalize();
        for( unsigned int j=0 ; j<6; ++j )
        {
            vn.push_back(v);
        }

        unsigned int k = i * 6;

        indices.push_back(k);
        indices.push_back(k+1);
        indices.push_back(k+2);

        indices.push_back(k);
        indices.push_back(k+2);
        indices.push_back(k+3);

        indices.push_back(k);
        indices.push_back(k+3);
        indices.push_back(k+4);

        indices.push_back(k);
        indices.push_back(k+4);
        indices.push_back(k+5);

        indices.push_back(k);
        indices.push_back(k+5);
        indices.push_back(k+1);
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( &vv[0], 72 );
        vas->setNormals( &vn[0], 72 );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createIcosahedron()
{
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    static const float x = 0.525731112119133606f;
    static const float z = 0.850650808352039932f;

    // Setup vertices
    static const Vec3f vertices[12] =
    {
        Vec3f(   -x , 0.0f ,    z ),
        Vec3f(    x , 0.0f ,    z ),
        Vec3f(   -x , 0.0f ,   -z ),
        Vec3f(    x , 0.0f ,   -z ),
        Vec3f( 0.0f ,    z ,    x ),
        Vec3f( 0.0f ,    z ,   -x ),
        Vec3f( 0.0f ,   -z ,    x ),
        Vec3f( 0.0f ,   -z ,   -x ),
        Vec3f(    z ,    x , 0.0f ),
        Vec3f(   -z ,    x , 0.0f ),
        Vec3f(    z ,   -x , 0.0f ),
        Vec3f(   -z ,   -x , 0.0f )
    };

    // Setup faces
    static const Face3 faces[20] =
    {
        { 0,  1,  4}, //  0
        { 0,  4,  9}, //  1
        { 0,  9, 11}, //  2
        { 0,  6,  1}, //  3
        { 0, 11,  6}, //  4
        { 1,  6, 10}, //  5
        { 1, 10,  8}, //  6
        { 1,  8,  4}, //  7
        { 2,  3,  7}, //  8
        { 2,  5,  3}, //  9
        { 2,  9,  5}, // 10
        { 2, 11,  9}, // 11
        { 2,  7, 11}, // 12
        { 3,  5,  8}, // 13
        { 3,  8, 10}, // 14
        { 3, 10,  7}, // 15
        { 4,  5,  9}, // 16
        { 4,  8,  5}, // 17
        { 6,  7, 10}, // 18
        { 6, 11,  7}  // 19
    };

    vector<Vec3f> vv;
    vector<Vec3f> vn;

    vv.reserve(60);
    vn.reserve(60);

    unsigned int j=0;

    vector<unsigned int> indices;
    vector<Face3> facesIco;
    indices.reserve( 60 );
    facesIco.reserve( 20 );

    for( unsigned int i = 0 ; i < 20; ++i )
    {
        Vec3f a = vertices[faces[i][0]];
        Vec3f b = vertices[faces[i][1]];
        Vec3f c = vertices[faces[i][2]];
        vv.push_back(a);
        vv.push_back(b);
        vv.push_back(c);

        Vec3f n = calculateFaceNormal( a, b, c );
        vn.push_back(n);
        vn.push_back(n);
        vn.push_back(n);

        indices.push_back(j++);
        indices.push_back(j++);
        indices.push_back(j++);
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( &vv[0], 60 );
        vas->setNormals( &vn[0], 60 );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createSphere( unsigned int m, unsigned int n, float radius )
{
    NVSG_ASSERT( m >= 3 && n >= 3 && "createSphere(): m and n both have to be at least 3." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals, indices/faces and texture coordinates
    vector< Vec3f > vertices;
    vector< Vec3f > tangents;
    vector< Vec3f > binormals;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face3 > faces;

    const int size_v = ( m + 1 ) * n;
    vertices.reserve( size_v );
    tangents.reserve( size_v );
    binormals.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( 6 * m * ( n - 1 ) );
    faces.reserve( 2 * m * ( n - 1 ) );

    float phi_step = 2.0f * PI / (float) m;
    float theta_step = PI / (float) (n - 1);

    // Latitudinal rings.
    // Starting at the south pole going upwards.
    for( unsigned int latitude = 0 ; latitude < n ; latitude++ ) // theta angle
    {
        float theta = (float) latitude * theta_step;
        float sinTheta = sinf( theta );
        float cosTheta = cosf( theta );
        float texv = (float) latitude / (float) (n - 1); // Range [0.0f, 1.0f]

        // Generate vertices along the latitudinal rings.
        // On each latitude there are m + 1 vertices,
        // the last one and the first one are on identical positions but have different texture coordinates.
        for( unsigned int longitude = 0 ; longitude <= m ; longitude++ ) // phi angle
        {
            float phi = (float) longitude * phi_step;
            float sinPhi = sinf( phi );
            float cosPhi = cosf( phi );
            float texu = (float) longitude / (float) m; // Range [0.0f, 1.0f]

            // Unit sphere coordinates are the normals.
            Vec3f v = Vec3f( cosPhi * sinTheta,
                             -cosTheta,                 // -y to start at the south pole.
                             -sinPhi * sinTheta );

            vertices.push_back( v * radius );
            texcoords.push_back( Vec2f( texu , texv ) );
            normals.push_back( v );
            tangents.push_back( Vec3f( -sinPhi, 0.0f, -cosPhi ) );
            binormals.push_back( Vec3f( cosTheta * cosPhi, sinTheta, cosTheta * -sinPhi ) );
        }
    }
    
    // We have generated m + 1 vertices per latitude.
    const unsigned int columns = m + 1;

    // Calculate indices
    for( unsigned int latitude = 0 ; latitude < n - 1 ; latitude++ )
    {
        for( unsigned int longitude = 0 ; longitude < m ; longitude++ )
        {
            indices.push_back(  latitude      * columns + longitude     );  // lower left
            indices.push_back(  latitude      * columns + longitude + 1 );  // lower right
            indices.push_back( (latitude + 1) * columns + longitude + 1 );  // upper right

            indices.push_back( (latitude + 1) * columns + longitude + 1 );  // upper right
            indices.push_back( (latitude + 1) * columns + longitude     );  // upper left
            indices.push_back(  latitude      * columns + longitude     );  // lower left
        }
    }

    // Create a VertexAttributeSet with vertices, normals and texcoords
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );

        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
        vas->setTexCoords( 0, &texcoords[0], size_v );
        vas->setTexCoords( VertexAttributeSet::NVSG_TANGENT  - VertexAttributeSet::NVSG_TEXCOORD0, &tangents[0], size_v );
        vas->setTexCoords( VertexAttributeSet::NVSG_BINORMAL - VertexAttributeSet::NVSG_TEXCOORD0, &binormals[0], size_v );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

DrawableSharedPtr createCylinder( float r, float h, unsigned int hdivs, unsigned int thdivs, bool bOuter, float thstart /*= 0.0f*/, float thend /*= 2.0f*nvmath::PI*/ )
{
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;

    unsigned int size_v = (hdivs+1)*thdivs + 2*thdivs + 2;

    // ---Vertex order---
    // 1) 1                 bottom center vert
    // 2) thdivs            bottom rim
    // 3) thdivs*(hdivs+1)  all height division circles including bottom and top rim
    // 4) 1                 top center point
    // 5) thdivs            top rim

    float th_step = (thend - thstart) / (float) thdivs;
    float h_step = h / (float) hdivs;

    //-------------------------------
    // Generate the vertices/normals
    //-------------------------------

    vertices.push_back( Vec3f( 0.0f, -h/2.0f, 0.0f ) );
    normals.push_back( Vec3f( 0.0f, bOuter ? -1.0f : 1.0f, 0.0f) );
    texcoords.push_back( Vec2f(0.5f, 0.5f) );

    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
        float curTh = thstart + ith*th_step;
        vertices.push_back( Vec3f( r*cosf(curTh), -h/2.0f, r*sinf(curTh) ) );
        normals.push_back( Vec3f( 0.0f, bOuter ? -1.0f : 1.0f, 0.0f) );
        texcoords.push_back( Vec2f( 0.5f*cos(curTh) + 0.5f, 0.5f*sin(curTh) + 0.5f) );
    }

    for(unsigned int ih = 0; ih < hdivs+1; ih++)
    {
        float curH = -h/2.0f + ih*h_step;

        for(unsigned int ith = 0; ith < thdivs; ith++)
        {
            float curTh = thstart + ith*th_step;

            vertices.push_back( Vec3f( r*cosf(curTh), curH, r*sinf(curTh)) );
            normals.push_back( Vec3f( (bOuter ? 1.0f : -1.0f)*cosf(curTh), 0.0f, (bOuter ? 1.0f : -1.0f)*sinf(curTh)) );
            texcoords.push_back( Vec2f( curTh/(thend - thstart), curH/h ) );
        }
    }

    vertices.push_back( Vec3f( 0.0f, h/2.0f, 0.0f ) );
    normals.push_back( Vec3f( 0.0f, bOuter ? 1.0f : -1.0f, 0.0f) );
    texcoords.push_back( Vec2f(0.5f, 0.5f) );

    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
        float curTh = thstart + ith*th_step;
        vertices.push_back( Vec3f( r*cosf(curTh), h/2.0f, r*sinf(curTh) ) );
        normals.push_back( Vec3f( 0.0f, bOuter ? 1.0f : -1.0f, 0.0f ) );
        texcoords.push_back( Vec2f( 0.5f*cos(curTh) + 0.5f, 0.5f*sin(curTh) + 0.5f) );
    }

    //-------------------------------
    // Generate the indices
    //-------------------------------

    int curvcount = 0;


    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
        indices.push_back( curvcount );
        indices.push_back( curvcount+1 + ith );
        indices.push_back( curvcount+2 + (ith < thdivs-1 ? ith : -1) );
    }



    curvcount += 1 + thdivs;

    for(unsigned int ih = 0; ih < hdivs; ih++)
    {
        for(unsigned int ith = 0; ith < thdivs; ith++)
        {
            indices.push_back( curvcount+ith );
            indices.push_back( curvcount+ith+thdivs );
            indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) );

            indices.push_back( curvcount+ith+thdivs );
            indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) + thdivs );
            indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) );
        }

        curvcount += thdivs;
    }

    curvcount += thdivs;


    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
        indices.push_back( curvcount );
        indices.push_back( curvcount+2 + (ith < thdivs-1 ? ith : -1) );
        indices.push_back( curvcount+1 + ith );
    }



    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );

        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
        vas->setTexCoords( 0, &texcoords[0], size_v );
    }

    IndexSetSharedPtr indexSet( IndexSet::create() );
    IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createTorus( unsigned int m, unsigned int n, float innerRadius , float outerRadius )
{
    // The torus is a ring with radius outerRadius rotated around the y-axis along the circle with innerRadius.
    
    /*           y
       ___       |       ___
     /     \           /     \
    |       |    |    |       |
    |       |         |       |
     \ ___ /     |     \ ___ /
                          <--->
                          outerRadius
                 <------->
                 innerRadius
    */

    NVSG_ASSERT( m >= 3 && n >= 3 && "createTorus(): m and n both have to be at least 3." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    vector< Vec3f > vertices;
    vector< Vec3f > tangents;
    vector< Vec3f > binormals;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face4 > faces;

    unsigned int size_v = ( m + 1 ) * ( n + 1 );

    vertices.reserve( size_v );
    tangents.reserve( size_v );
    binormals.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( 4 * m * n );
    faces.reserve( m * n );

    float mf = (float) m;
    float nf = (float) n;

    float phi_step   = 2.0f * PI / mf;
    float theta_step = 2.0f * PI / nf;

    // Setup vertices and normals
    // Generate the Torus exactly like the sphere with rings around the origin along the latitudes.
    for ( unsigned int latitude = 0; latitude <= n; latitude++ ) // theta angle
    {
        float theta = (float) latitude * theta_step;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        float radius = innerRadius + outerRadius * cosTheta;

        for ( unsigned int longitude = 0; longitude <= m; longitude++ ) // phi angle
        {
            float phi = (float) longitude * phi_step;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            vertices.push_back( Vec3f( radius      *  cosPhi,
                                       outerRadius *  sinTheta,
                                       radius      * -sinPhi ) );

            tangents.push_back( Vec3f( -sinPhi, 0.0f, -cosPhi ) );

            binormals.push_back( Vec3f( cosPhi * -sinTheta,
                                        cosTheta,
                                        sinPhi * sinTheta ) );

            normals.push_back( Vec3f( cosPhi * cosTheta,
                                      sinTheta,
                                      -sinPhi * cosTheta ) );

            texcoords.push_back( Vec2f( (float) longitude / mf , (float) latitude / nf ) );
        }
    }

    const unsigned int columns = m + 1;

    // Setup indices
    for( unsigned int latitude = 0 ; latitude < n ; latitude++ )
    {
        for( unsigned int longitude = 0 ; longitude < m ; longitude++ )
        {
            indices.push_back(  latitude      * columns + longitude     );  // lower left
            indices.push_back(  latitude      * columns + longitude + 1 );  // lower right
            indices.push_back( (latitude + 1) * columns + longitude + 1 );  // upper right
            indices.push_back( (latitude + 1) * columns + longitude     );  // upper left
        }
    }
    
    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
        vas->setTexCoords( 0, &texcoords[0], size_v );
        vas->setTexCoords( VertexAttributeSet::NVSG_TANGENT  - VertexAttributeSet::NVSG_TEXCOORD0, &tangents[0], size_v );
        vas->setTexCoords( VertexAttributeSet::NVSG_BINORMAL - VertexAttributeSet::NVSG_TEXCOORD0, &binormals[0], size_v );
    }
    
    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_QUADS );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createTessellatedPlane( unsigned int subdiv, const Mat44f &transf )
{
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices, normals, faces and texture coordinates (and indices for primitive creation mode)
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face3 > faces;

    const int size_v =  ( subdiv + 2 ) * ( subdiv + 2 );
    vertices.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( 2 * ( subdiv + 1 ) * ( subdiv + 1 ) );

    // Setup tessellated plane
    setupTessellatedPlane( subdiv, transf, faces, vertices, normals, texcoords, indices);

    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
        vas->setTexCoords( 0, &texcoords[0], size_v );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createPlane( float x0, float y0,
                               float width, float height,
                               float wext, float hext)
{
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;

    vertices.push_back( Vec3f(x0, y0, 0.0f) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(0.0f, 0.0f) );

    vertices.push_back( Vec3f(x0 + width, y0, 0.0f) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(wext, 0.0f) );

    vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(wext, hext) );

    /////

    vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(wext, hext) );

    vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(0.0f, hext) );

    vertices.push_back( Vec3f(x0, y0, 0.0f) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(0.0f, 0.0f) );


    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );

        vas->setVertices( &vertices[0], 6 );
        vas->setNormals( &normals[0], 6 );
        vas->setTexCoords( 0, &texcoords[0], 6 );
        vas->setTexCoords( 1, &texcoords[0], 6 );
    }

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
}

// ===========================================================================

DrawableSharedPtr createTessellatedBox( unsigned int subdiv )
{
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices, normals, faces and texture coordinates
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face3 > faces;

    const int size_v = 6 * ( subdiv + 2 ) * ( subdiv + 2 );
    vertices.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( 6 * 2 * ( ( subdiv + 1 ) * ( subdiv + 1 ) ) );

    // Setup transformations for 6 box sides
    Mat44f transf[6];
    transf[0] = Mat44f( 1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  1.0f ); // front

    transf[1] = Mat44f(-1.0f,  0.0f,  0.0f,  0.0f,
                       0.0f,  1.0f,  0.0f,  0.0f,
                       0.0f,  0.0f, -1.0f,  0.0f,
                       0.0f,  0.0f, -1.0f,  1.0f ); // back, 180 degrees around y-axis

    transf[2] = Mat44f( 0.0f,  0.0f,  1.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        -1.0f,  0.0f,  0.0f,  0.0f,
                        -1.0f,  0.0f,  0.0f,  1.0f ); // left, -90 degrees around y-axis

    transf[3] = Mat44f( 0.0f,  0.0f, -1.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        1.0f,  0.0f,  0.0f,  0.0f,
                        1.0f,  0.0f,  0.0f,  1.0f ); // right, 90 degrees around y-axis

    transf[4] = Mat44f( 1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  1.0f,
                        0.0f, -1.0f,  0.0f,  0.0f,
                        0.0f, -1.0f,  0.0f,  1.0f ); // bottom, 90 degrees around x-axis

    transf[5] = Mat44f( 1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  0.0f, -1.0f,  1.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  1.0f ); // top, -90 degrees around x-axis
    
    for ( unsigned int i=0; i<6; i++ )
    {
        setupTessellatedPlane( subdiv, transf[i], faces, vertices, normals, texcoords, indices);
    }

    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
        VertexAttributeSetWriteLock vas( vasPtr );
        vas->setVertices( &vertices[0], size_v );
        vas->setNormals( &normals[0], size_v );
        vas->setTexCoords( 0, &texcoords[0], size_v );
    }

    {
        // Create a PrimitiveSet
        IndexSetSharedPtr indexSet( IndexSet::create() );
        IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

        PrimitiveSharedPtr primitivePtr = Primitive::create();
        {
            PrimitiveWriteLock primitive(primitivePtr);
            primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
            primitive->setVertexAttributeSet( vasPtr );
            primitive->setIndexSet( indexSet );
        }

        drawablePtr = primitivePtr;
    }

    return drawablePtr;
}

// ===========================================================================

StateSetSharedPtr createDefaultMaterial( const Vec3f &diffuseColor )
{
    // Create a Material
    MaterialSharedPtr materialPtr = Material::create();
    StateSetSharedPtr statePtr = StateSet::create();
    MaterialWriteLock mw( materialPtr );
    mw->setDiffuseColor( diffuseColor );

    // Create a StateSet
    StateSetWriteLock ssw( statePtr );
    ssw->addAttribute( materialPtr );

    return statePtr;
}

// ===========================================================================

StateSetSharedPtr createDefinedMaterial( const Vec3f &ambientColor,
                                         const Vec3f &diffuseColor,
                                         const Vec3f &specularColor,
                                         const float specularExponent,
                                         const Vec3f &emissiveColor,
                                         const float opacity,
                                         const float reflectivity,
                                         const float indexOfRefraction )
{
    // Create a Material
    MaterialSharedPtr materialPtr = createMaterialAttribute( ambientColor,
                                                             diffuseColor,
                                                             specularColor,
                                                             specularExponent,
                                                             emissiveColor,
                                                             opacity,
                                                             reflectivity,
                                                             indexOfRefraction );

    // Create a StateSet
    StateSetSharedPtr statePtr = StateSet::create();
    StateSetWriteLock( statePtr )->addAttribute( materialPtr );

    return statePtr;
}

// ===========================================================================

MaterialSharedPtr createMaterialAttribute( const Vec3f &ambientColor,
                                           const Vec3f &diffuseColor,
                                           const Vec3f &specularColor,
                                           const float specularExponent,
                                           const Vec3f &emissiveColor,
                                           const float opacity,
                                           const float reflectivity,
                                           const float indexOfRefraction )
{
    // Create a Material
    MaterialSharedPtr materialPtr = Material::create();
    {
        MaterialWriteLock mw( materialPtr );
        mw->setAmbientColor( ambientColor );
        mw->setDiffuseColor( diffuseColor );
        mw->setSpecularColor( specularColor );
        mw->setSpecularExponent( specularExponent );
        mw->setEmissiveColor( emissiveColor );
        mw->setOpacity( opacity );
        mw->setReflectivity( reflectivity );
        mw->setIndexOfRefraction( indexOfRefraction );
    }
    return materialPtr;
}

// ===========================================================================

StateSetSharedPtr createTextureFromFile(std::string &fileName, std::vector<std::string> &searchPaths)
{
    nvutil::addPlugInSearchPath(std::string("C:\\Program Files\\NVIDIA Corporation\\SceniX 7.2\\bin\\x86\\win\\crt90\\debug"));
    nvutil::addPlugInSearchPath(searchPaths[0]);
    nvutil::addPlugInSearchPath(searchPaths[1]);

    std::vector<std::string> pluginSearchPaths = nvutil::getPlugInSearchPath();

    TextureHostSharedPtr tisp = TextureHost::createFromFile(fileName, searchPaths,
                                                                TextureHost::F_SCALE_FILTER_BOX);

    {
        TextureHostWriteLock tihwl(tisp);
        tihwl->setTextureTarget(determineTextureTarget(tisp));
    }

    TextureAttributeSharedPtr texAttPtr( TextureAttribute::create() );
    {
        TextureAttributeWriteLock texAtt( texAttPtr );
        TextureAttributeItemSharedPtr texAttItemPtr( TextureAttributeItem::create() );
        TextureAttributeItemWriteLock texAttItem( texAttItemPtr );
        texAttItem->setTexture( tisp );
        texAttItem->setMagFilterMode( TFM_MAG_NEAREST );
        texAttItem->setMinFilterMode( TFM_MIN_NEAREST );
        texAtt->bindTextureAttributeItem( texAttItemPtr, 0 );
    }

    // Create a StateSet and attach material and shader
    StateSetSharedPtr stateSetPtr = StateSet::create();
    {
        StateSetWriteLock stateSet( stateSetPtr );
        stateSet->addAttribute( texAttPtr );
    }
    return stateSetPtr;
}

// ===========================================================================

StateSetSharedPtr createTexture()
{
    vector<Vec4f> tex;
    tex.resize(64);

    // Create pattern
    for( unsigned int i = 0; i < 8; ++i )
    {
        for( unsigned int j = 0; j < 8; ++j )
        {
            unsigned int pos = i * 8 + j;
            Vec4f col(float(( i ^ j ) & 1), float((( i ^ j ) & 2) / 2), float(((i  ^ j ) & 4) / 4), 1.0f);
            tex.at(pos) = col;
        }
    }

    TextureHostSharedPtr tisp = TextureHost::create();
    {
        TextureHostWriteLock tiw( tisp );
        tiw->setCreationFlags( TextureHost::F_PRESERVE_IMAGE_DATA_AFTER_UPLOAD );
        unsigned int index = tiw->addImage( 8, 8, 1, Image::IMG_RGBA, Image::IMG_FLOAT32 );
        NVSG_ASSERT( index != -1 );
        tiw->setImageData( index, (const void *) &tex[0] );

        tiw->setTextureTarget( NVSG_TEXTURE_2D );
        tiw->setTextureGPUFormat(TextureHost::TGF_FIXED8);
    }

    TextureAttributeSharedPtr texAttPtr( TextureAttribute::create() );
    {
        TextureAttributeWriteLock texAtt( texAttPtr );
        TextureAttributeItemSharedPtr texAttItemPtr( TextureAttributeItem::create() );
        TextureAttributeItemWriteLock texAttItem( texAttItemPtr );
        texAttItem->setTexture( tisp );
        texAttItem->setMagFilterMode( TFM_MAG_NEAREST );
        texAttItem->setMinFilterMode( TFM_MIN_NEAREST );
        texAtt->bindTextureAttributeItem( texAttItemPtr, 0 );
    }

    // Create a StateSet and attach material and shader
    StateSetSharedPtr stateSetPtr = StateSet::create();
    {
        StateSetWriteLock stateSet( stateSetPtr );
        stateSet->addAttribute( texAttPtr );
    }

    return stateSetPtr;
}

// ===========================================================================

StateSetSharedPtr createAlphaTexture( unsigned int n /* =64 */ )
{
    vector<Vec4f> tex;
    tex.resize(n*n);
    
    // Create pattern
    for( unsigned int i = 0; i < n; ++i )
    {
        for( unsigned int j = 0; j < n; ++j )
        {
            float dx = ( (float)i - (float)(n-1)/2.0f ) / ( (float)(n-1)/2.0f );
            float dy = ( (float)j - (float)(n-1)/2.0f ) / ( (float)(n-1)/2.0f );

            float val = max(0.0f, 1.0f - (dx*dx+dy*dy));

            unsigned int pos = i * n + j;
            Vec4f col( val, val, val, val );
            tex.at(pos) = col;
        }
    }

    TextureHostSharedPtr tisp = TextureHost::create();
    {
        TextureHostWriteLock tiw( tisp );
        tiw->setCreationFlags( TextureHost::F_PRESERVE_IMAGE_DATA_AFTER_UPLOAD );
        unsigned int index = tiw->addImage( n, n, 1, Image::IMG_RGBA, Image::IMG_FLOAT32 );
        NVSG_ASSERT( index != -1 );
        tiw->setImageData( index, (const void *) &tex[0] );
        tiw->setTextureTarget( NVSG_TEXTURE_2D );
        tiw->setTextureGPUFormat(TextureHost::TGF_FIXED8);
    }

    TextureAttributeSharedPtr texAttPtr( TextureAttribute::create() );
    {
        TextureAttributeWriteLock texAtt( texAttPtr );
        TextureAttributeItemSharedPtr texAttItemPtr( TextureAttributeItem::create() );
        TextureAttributeItemWriteLock texAttItem( texAttItemPtr );
        texAttItem->setTexture( tisp );
        texAttItem->setMagFilterMode( TFM_MAG_NEAREST );
        texAttItem->setMinFilterMode( TFM_MIN_NEAREST );
        texAtt->bindTextureAttributeItem( texAttItemPtr, 0 );
    }

    // Create a StateSet and attach material and shader
    StateSetSharedPtr stateSetPtr = StateSet::create();
    {
        StateSetWriteLock stateSet( stateSetPtr );
        stateSet->addAttribute( texAttPtr );
    }

    return stateSetPtr;
}

// ===========================================================================

GeoNodeSharedPtr createGeoNode( const DrawableSharedPtr &drawable, const StateSetSharedPtr &stateSet )
{
    //Create a GeoNode combining StateSet and Shape
    GeoNodeSharedPtr geoPtr;
    geoPtr = GeoNode::create();
    GeoNodeWriteLock gnw( geoPtr );
    gnw->addDrawable( stateSet, drawable );

    return geoPtr;
}

// ===========================================================================

TransformSharedPtr createTransform( const NodeSharedPtr &node , const Vec3f &translation, const Quatf &orientation, const Vec3f &scaling )
{
    // Make a Transformation
    Trafo trafo;
    trafo.setTranslation( translation );
    trafo.setOrientation( orientation );
    trafo.setScaling( scaling );

    // Create a Transform
    TransformSharedPtr transPtr = Transform::create();
    TransformWriteLock tw( transPtr );
    tw->setTrafo( trafo );
    tw->addChild( node );

    return transPtr;
}

// ===========================================================================

TransformSharedPtr imitateRaster( const NodeSharedPtr &node, unsigned int width, unsigned int height )
{
    Trafo trafo;

    trafo.setTranslation( Vec3f( -0.5f*width/height,  -0.5f,  0.0f ) );
    trafo.setOrientation( Quatf( Vec3f(0.0, 1.0, 0.0), 0.0) );
    trafo.setScaling( Vec3f( 1.0f/height, 1.0f/height, 1.0f ) );

    TransformSharedPtr transPtr = Transform::create();
    TransformWriteLock tw( transPtr );
    tw->setTrafo( trafo );
    tw->addChild( node );

    return transPtr;
}

// ===========================================================================

TransformSharedPtr imitateRaster( unsigned int width, unsigned int height )
{
    Trafo trafo;

    trafo.setTranslation( Vec3f( -0.5f*width/height,  -0.5f,  0.0f ) );
    trafo.setOrientation( Quatf( Vec3f(0.0, 1.0, 0.0), 0.0) );
    trafo.setScaling( Vec3f( 1.0f/height, 1.0f/height, 1.0f ) );


    TransformSharedPtr transPtr = Transform::create();
    TransformWriteLock tw( transPtr );
    tw->setTrafo( trafo );

    return transPtr;
}

// ===========================================================================

void setCameraPOV(float x, float y, float z, ViewStateSharedPtr viewState)
{

    Vec3f pos(x, y, z);

    ViewStateWriteLock theViewState(viewState);
    CameraWriteLock theCamera(theViewState->getCamera());

    Vec3f curpov = theCamera->getPosition();

    theCamera->setPosition( pos );
}

// ===========================================================================

void setCameraDirNoRoll(float x, float y, float z, ViewStateSharedPtr viewState)
{

    Vec3f dir(x, y, z);

    ViewStateWriteLock theViewState(viewState);
    CameraWriteLock theCamera(theViewState->getCamera());

    dir.normalize();

    theCamera->setDirection( dir );
    theCamera->setUpVector( Vec3f(0.0f, 1.0f, 0.0f) );
}

// ===========================================================================

void setCameraDir(float x, float y, float z, ViewStateSharedPtr viewState)
{

    Vec3f dir(x, y, z);

    ViewStateWriteLock theViewState(viewState);
    CameraWriteLock theCamera(theViewState->getCamera());

    dir.normalize();

    Vec3f up;

    if( ( dir != Vec3f(0.0f, 1.0f, 0.0f) ) && ( dir != Vec3f(0.0f, -1.0f, 0.0f) ) )
    {
        Vec3f yup(0.0f, 1.0f, 0.0f);

        up = yup - dir*(dir*yup);
        up.normalize();
    }
    else
    {
        up = Vec3f(0.0f, 0.0f, dir[1]);
    }

    theCamera->setUpVector( up );
    theCamera->setDirection( dir );
}

// ===========================================================================

DirectedLightSharedPtr createDirectedLight( const Vec3f &direction , const Vec3f &diffuseColor )
{
    DirectedLightSharedPtr directedLightPtr = DirectedLight::create();
    DirectedLightWriteLock dlw( directedLightPtr );
    dlw->setAmbientColor(  Vec3f( 1.0f, 1.0f, 1.0f ) );
    dlw->setDiffuseColor( diffuseColor );
    dlw->setSpecularColor( Vec3f( 1.0f, 1.0f, 1.0f ) );
    Vec3f dir = direction;
    dir.normalize();
    dlw->setDirection( dir );

    return directedLightPtr;
}

// ===========================================================================

PointLightSharedPtr createPointLight( const Vec3f &position , const Vec3f &diffuseColor )
{
    PointLightSharedPtr pointLightPtr = PointLight::create();
    PointLightWriteLock plw( pointLightPtr );
    plw->setAmbientColor(  Vec3f( 1.0f, 1.0f, 1.0f ) );
    plw->setDiffuseColor( diffuseColor );
    plw->setSpecularColor( Vec3f( 1.0f, 1.0f, 1.0f ) );
    plw->setPosition( position );

    return pointLightPtr;
}

// ===========================================================================

SpotLightSharedPtr createSpotLight( const Vec3f &position , const Vec3f &direction , const float angle , const Vec3f &diffuseColor )
{
    SpotLightSharedPtr spotLightPtr = SpotLight::create();
    SpotLightWriteLock slw( spotLightPtr );
    slw->setPosition( position );
    Vec3f dir = direction;
    dir.normalize();
    slw->setDirection( dir );
    slw->setCutoffAngle( angle );
    slw->setAmbientColor(  Vec3f( 1.0f, 1.0f, 1.0f ) );
    slw->setDiffuseColor( diffuseColor );
    slw->setSpecularColor( Vec3f( 1.0f, 1.0f, 1.0f ) );

    return spotLightPtr;
}

} //namespace nvutil

