// Copyright NVIDIA Corporation 2009-2010
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#pragma once
/** \file */

#include <nvsg/CoreTypes.h>
#include <nvtraverser/RayIntersectTraverser.h>

namespace nvutil
{
/*! \brief Load a scene, internally doing all the SceneLoader handling.
   *  \param filename The name of the file to load.
   *  \param searchPaths Optional array of search paths to find the file to load.
   *  \param callback Optional pointer to a callback used by the loader for the file to load.
   *  \return A ViewState containing loaded scene on success and a nullptr ViewState otherwise.
   *  \sa saveScene */
nvsg::ViewStateSharedPtr loadScene( const std::string & filename
                                    , const std::vector<std::string> &searchPaths = std::vector<std::string>()
        , PlugInCallback *callback = 0 );

/*! \brief Save a scene, internally doing all the SceneSaver handling.
   *  \param filename The name of the file to save to.
   *  \param viewState The view state (holding the scene) to save.
   *  \param callback Optional pointer to a callback used by the saver for the file to save.
   *  \return true if a scene could be saved, false otherwise
   *  \sa loadScene */
bool saveScene( const std::string & filename
                , const nvsg::ViewStateSharedPtr & viewState
                , PlugInCallback *callback = 0 );

/** Initialize the given viewState with defaults. Choose the first camera of the scene.
    * If there's no camera in the scene create a new perspective camera and perform zoomAll.
    * Create a RendererOptions object if none is in the viewState. **/
bool setupDefaultViewState( const nvsg::ViewStateSharedPtr & viewState );

/*! \brief Add a headlight to the camera in case there is none.
   *  \param camera The PerspectiveCamera to which to add the headlight.
   *  \param offset Place the headlight relative to the camera position with this offset in local camera space.
   *  \return true if a new cameras has been added, false otherwise
   **/
bool createDefaultHeadLight( const nvsg::CameraSharedPtr & camera
                             , nvmath::Vec3f offset = nvmath::Vec3f(0.0f, 0.0f, 0.0f) );

/*! \brief Optimize the given scene specified by the given flags
   *  \param scene The scene which to optimize.
   *  \param ignoreNames If \c true, optimizing ingores names of objects.
   *  \param identityToGroup If \c true, the IdentityToGroupTraverser is executed.
   *  \param combineFlags The flags to use for the CombineTraverser.
   *  \param eliminateFlags The flags to use for the EliminateTraverser.
   *  \param unifyFlags The flags to use for the UnifyTraverser.
   *  \param epsilon The epsilon value to use to identify unique vertices while running the UnifyTraverser.
   **/
void optimizeScene( const nvsg::SceneSharedPtr & scene, bool ignoreNames, bool identityToGroup
                    , unsigned int combineFlags, unsigned int eliminateFlags, unsigned int unifyFlags
                    , float epsilon );

/*! \brief optimize the given scene for optimal raytracing performance
   *  \param scene The Scene which is going to be optimized.
   **/
void optimizeForRaytracing( const nvsg::SceneSharedPtr & scene );

/*! \brief Merge nearby vertices. Note that this can be a very expensive operation.
   *  \param scene The Scene which is going to be optimized.
   **/
void optimizeUnifyVertices( const nvsg::SceneSharedPtr & scene );

/*! \brief Get the distance to the closest object
   * \param viewState ViewState describing camera setup
   * \param renderTarget describing the viewport window
   * \param windowX x position of mouse inside the viewport window
   * \param windowY y position of mouse inside the viewport window
   * \return float distance to closest object.  -1 if nothing intersected.
   */
float getIntersectionDistance( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget,
                               int windowX, int windowY );

/*! \brief intersect the scene from a window-space point
   * \param viewState ViewState describing the camera setup
   * \param renderTarget describing the viewport window
   * \param windowX x position of mouse inside the viewport window
   * \param windowY y position of mouse inside the viewport window
   * \param baseSearch the base node in the scene to search from
   * \param result the nvtraverser::Intersection result, if the method returns true
   * \return true if an intersection was found
   */
bool intersectObject( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget,
                      unsigned int windowX, unsigned int windowY, nvtraverser::Intersection & result );

/*! \brief Save a texture image to disk
   * \param filename disk file to save image to
   * \param tih texture image to save
   * \return true if save was successful
   */
bool saveTextureHost( const std::string & filename, const nvsg::TextureHostSharedPtr & tih );

/*! \brief Load a texture image from disk
   * \param filename disk file to load image from
   * \param tih texture image handle to load to
   * \param searchPaths additional search paths
   * \return true if save was successful
   */
bool loadTextureHost( const std::string & filename, nvsg::TextureHostSharedPtr & tih
                      , const std::vector<std::string> &searchPaths = std::vector<std::string>() );
} // namespace nvutil
