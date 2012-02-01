/*
\brief Generator to generate a series of testing scenes
*/

#pragma once

#include <nvsg/CoreTypes.h>
#include <nvmath/Vecnt.h>
#include <nvmath/Quatt.h>

namespace nvutil
{
//! Generate a quadmesh with m x n quads, m rows and n columns
// supported attributes: vertex, normal
nvsg::DrawableSharedPtr createQuadSet( unsigned int m, unsigned int n, const float size = 1.0f, const float gap  = 0.5f );

//! Generate a quad strip with n quads
// supported attributes: vertex, normal
nvsg::DrawableSharedPtr createQuadStrip( unsigned int n, float height = 1.0f , float radius = 1.0f );

//! Generate a triangle mesh with m x n triangles, m rows and n columns
// supported attributes: vertex, normal
nvsg::DrawableSharedPtr createTriSet( unsigned int m, unsigned int n, const float size = 1.0f, const float gap  = 0.5f );

//! Generate a half-circle trifan with n segments
// supported attributes: vertex, normal
nvsg::DrawableSharedPtr createTriFan( unsigned int n, const float radius = 1.0f, const float elevation = 0.0f );

//! Generate \a rows rows and \a columns columns of tri strips with the width \a width and the height \a height of one triangle pair
// supported attributes: vertex, normal
nvsg::DrawableSharedPtr createTriStrip( unsigned int rows, unsigned int columns, float width = 1.0f, float height = 1.0f );

//! Generate a GeoNode with a TriPatches4 with n x m triangles, m rows and n columns, each of size \a size, separated by \a offset
nvsg::GeoNodeSharedPtr createTriPatches4( const std::vector<std::string> & searchPaths
                                          , unsigned int n, unsigned int m
                                          , const nvmath::Vec3f & size = nvmath::Vec3f( 4.0f, 4.0f, 4.0f )
        , const nvmath::Vec2f & offset = nvmath::Vec2f( 4.0f, 4.0f ) );

//! Generate a GeoNode with a QuadPatches4x4 with n x m cylinders, seperated by \a offset
nvsg::GeoNodeSharedPtr createQuadPatches4x4( const std::vector<std::string> & searchPaths
                                             , unsigned int n, unsigned int m
                                             , const nvmath::Vec2f & offset = nvmath::Vec2f( 4.0f, 4.0f ) );

//! Generate a unit cube inside the range [-1, 1]
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createCube();

//! Generate a tetrahedron
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createTetrahedron();

//! Generate an octahedron
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createOctahedron();

//! Generate a dodecahedron
// supported attributes: vertex, normal
nvsg::DrawableSharedPtr createDodecahedron();

//! Generate an icosahedron
// supported attributes: vertex, normal
nvsg::DrawableSharedPtr createIcosahedron();

//! Generate a sphere around (0,0,0) with a radius of \a radius, m edges in longitudinal and n edges in latitudinal direction, both m and n should be at least 3
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createSphere( unsigned int m , unsigned int n , float radius = 1.0f );

//! Generate a cylinder with a radius of size \a radius, m edges in longitudinal and n edges in latitudinal direction, both m and n should be at least 3
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createCylinder( float r, float h, unsigned int hdivs, unsigned int thdivs, bool bOuter = true, float thstart = 0.0f, float thend = 2.0f*nvmath::PI );

//! Generate a torus with m edges in longitudinal and n edges in latitudinal direction, an inner radius of \a innerRadius and an outer radius of \a outerRadius
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createTorus( unsigned int m, unsigned int n, float innerRadius = 1.0f, float outerRadius = 0.5f );

//! Generate a tessellated plane with \a subdiv subdivisions and the possibility to apply a transformation matrix transf
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createTessellatedPlane( unsigned int subdiv, const nvmath::Mat44f &transf = nvmath::Mat44f( 1.0f, 0.0f, 0.0f, 0.0f,
                                                                                                                    0.0f, 1.0f, 0.0f, 0.0f,
                                                                                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                                                                                    0.0f, 0.0f, 0.0f, 1.0f ) );

/*! Create an XY aligned plane that could conveniantly be used to make pixel-aligned rectangles: ( \a x0, \a y0 ) - the bottom left corner of the rect,
  and the width \a width in pixels, and the height \a height in pixels of the rect. \a wext and \a hext can be set to specify how far
  the texture coordinates extend at the top right corner of the rect*/
nvsg::DrawableSharedPtr createPlane( float x0, float y0, float width, float height, float wext = 1.0f, float hext = 1.0f);

//! Generate a tessellated box out of tessellated planes with \a subdiv subdivisions
// supported attributes: vertex, normal, texcoord0 (2D)
nvsg::DrawableSharedPtr createTessellatedBox( unsigned int subdiv );

/*! Generate a default material with the diffuse color \a diffuseColor,
      an ambient color of (0.2f,0.2f,0.2f), a specular color of (0.0f,0.0f,0.0f), a specular exponent of 0.0f,
      an emissive color of (0.0f,0.0f,0.0f) and an opacity of 1.0f */
nvsg::StateSetSharedPtr createDefaultMaterial( const nvmath::Vec3f &diffuseColor );

/*! Generate a material with the ambient color \a ambientColor, the diffuse color \a diffuseColor, the specular color \a specularColor,
      the specular exponent \a specularExponent, the emissive color \a emissiveColor, the opacity \a opacity,  the reflectivity \a reflectivity
      and the index of refraction \a indexOfRefraction */
nvsg::StateSetSharedPtr createDefinedMaterial( const nvmath::Vec3f &ambientColor,
                                               const nvmath::Vec3f &diffuseColor,
                                               const nvmath::Vec3f &specularColor,
                                               const float specularExponent,
                                               const nvmath::Vec3f &emissiveColor,
                                               const float opacity = 1.0f,
                                               const float reflectivity = 0.0f,
                                               const float indexOfRefraction = 1.0f );

/*! Generate a material with the ambient color \a ambientColor, the diffuse color \a diffuseColor, the specular color \a specularColor,
      the specular exponent \a specularExponent, the emissive color \a emissiveColor, the opacity \a opacity,  the reflectivity \a reflectivity
      and the index of refraction \a indexOfRefraction */
nvsg::MaterialSharedPtr createMaterialAttribute( const nvmath::Vec3f &ambientColor,
                                                 const nvmath::Vec3f &diffuseColor,
                                                 const nvmath::Vec3f &specularColor,
                                                 const float specularExponent,
                                                 const nvmath::Vec3f &emissiveColor,
                                                 const float opacity = 1.0f,
                                                 const float reflectivity = 0.0f,
                                                 const float indexOfRefraction = 1.0f );

//! Generate a texture from the given fileName
nvsg::StateSetSharedPtr createTextureFromFile(std::string &fileName, std::vector<std::string> &searchPaths);

//! Generate a texture of size 8x8 with a colored checker pattern
nvsg::StateSetSharedPtr createTexture();

//! Generate a texture of size n x n with grey and alpha values
nvsg::StateSetSharedPtr createAlphaTexture( unsigned int n=64 );

//! Generate a GeoNode from a shape \a drawable and a state set \a stateSet
nvsg::GeoNodeSharedPtr createGeoNode( const nvsg::DrawableSharedPtr &drawable, const nvsg::StateSetSharedPtr &stateSet );

//! Generate a transformation for a node \a node with a translation vector \a translation as well as an orientation vector \a orientation
nvsg::TransformSharedPtr createTransform( const nvsg::NodeSharedPtr &node,
                                          const nvmath::Vec3f &translation = nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),
                                          const nvmath::Quatf &orientation = nvmath::Quatf( nvmath::Vec3f( 0.0f, 1.0f, 0.0f ) , 0.0f ),
                                          const nvmath::Vec3f &scaling = nvmath::Vec3f( 1.0f, 1.0f, 1.0f ) );

//! Generate a transformation for a node that maps x, y coordinates as if they are direct window space coordinates
nvsg::TransformSharedPtr imitateRaster( const nvsg::NodeSharedPtr &node, unsigned int width, unsigned int height);

//! Generate a transformation that maps x, y coordinates as if they are direct window space coordinates
nvsg::TransformSharedPtr imitateRaster( unsigned int width, unsigned int height );

//! Sets the point of view of the camera of the given view state
void setCameraPOV(float x, float y, float z, nvsg::ViewStateSharedPtr viewState);

//! Sets the direction of the camera of the given view state with up vector always point in positive y direction
void setCameraDirNoRoll(float x, float y, float z, nvsg::ViewStateSharedPtr viewState);

//! Sets the direction of the camera of the given view state
void setCameraDir(float x, float y, float z, nvsg::ViewStateSharedPtr viewState);

/*! Generate a directed lighting with the direction \a direction and the diffuse color \a diffuseColor,
      an ambient color of (1.0f,1.0f,1.0f), a specular color of (0.0f,0.0f,0.0f), a specular exponent of 0.0f */
nvsg::DirectedLightSharedPtr createDirectedLight( const nvmath::Vec3f &direction = nvmath::Vec3f( 1.0f, -1.0f, -1.0f ),
                                                  const nvmath::Vec3f &diffuseColor = nvmath::Vec3f( 1.0f, 1.0f, 1.0f ) );

/*! Generate a point light with the position \a position, the diffuse color \a diffuseColor,
      an ambient color of (1.0f,1.0f,1.0f), a specular color of ( 1.0f,1.0f,1.0f ), a specular exponent of 0.0f */
nvsg::PointLightSharedPtr createPointLight( const nvmath::Vec3f &position,
                                            const nvmath::Vec3f &diffuseColor = nvmath::Vec3f( 1.0f, 1.0f, 1.0f ) );

/*! Generate a spot light with the position \a position, direction \direction, cutoff angle \angle,
      the diffuse color \a diffuseColor, an ambient color of (1.0f,1.0f,1.0f),
      a specular color of ( 1.0f,1.0f,1.0f )
      */
nvsg::SpotLightSharedPtr createSpotLight( const nvmath::Vec3f &position , const nvmath::Vec3f &direction , const float angle,
                                          const nvmath::Vec3f &diffuseColor = nvmath::Vec3f( 1.0f, 1.0f, 1.0f ) );
} // namespace nvutil

