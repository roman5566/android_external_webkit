/*
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TilesManager_h
#define TilesManager_h

#if USE(ACCELERATED_COMPOSITING)

#include "BackedDoubleBufferedTexture.h"
#include "BaseTile.h"
#include "LayerAndroid.h"
#include "LayerTexture.h"
#include "ShaderProgram.h"
#include "TexturesGenerator.h"
#include "TiledPage.h"
#include <utils/threads.h>

namespace WebCore {

class TileSet;

class TilesManager {
public:
    static TilesManager* instance();

    void removeOperationsForPage(TiledPage* page)
    {
        m_pixmapsGenerationThread->removeOperationsForPage(page);
    }

    void removeOperationsForBaseLayer(BaseLayerAndroid* layer)
    {
        m_pixmapsGenerationThread->removeOperationsForBaseLayer(layer);
    }

    void removeOperationsForTexture(LayerTexture* texture)
    {
        m_pixmapsGenerationThread->removeOperationsForTexture(texture);
    }

    void scheduleOperation(QueuedOperation* operation)
    {
        m_pixmapsGenerationThread->scheduleOperation(operation);
    }

    ShaderProgram* shader() { return &m_shader; }

    BackedDoubleBufferedTexture* getAvailableTexture(BaseTile* owner);

    void printLayersTextures(const char* s);
    void cleanupLayersTextures(LayerAndroid* layer, bool forceCleanup = false);
    LayerTexture* getExistingTextureForLayer(LayerAndroid* layer);
    LayerTexture* createTextureForLayer(LayerAndroid* layer);

    void markGeneratorAsReady()
    {
        android::Mutex::Autolock lock(m_generatorLock);
        m_generatorReadyCond.signal();
    }

    void printTextures();
    void enableTextures();

    void resetTextureUsage(TiledPage* page);
    void paintTexturesDefault();

    static int maxTextureCount();
    static float tileWidth();
    static float tileHeight();

private:

    TilesManager();

    void waitForGenerator()
    {
        android::Mutex::Autolock lock(m_generatorLock);
        m_generatorReadyCond.wait(m_generatorLock);
    }

    Vector<BackedDoubleBufferedTexture*> m_textures;
    Vector<LayerTexture*> m_layersTextures;

    unsigned int m_layersMemoryUsage;
    GLint m_maxTextureSize;
    unsigned int m_totalMaxTextureSize;

    bool m_generatorReady;

    sp<TexturesGenerator> m_pixmapsGenerationThread;

    android::Mutex m_texturesLock;
    android::Mutex m_generatorLock;
    android::Condition m_generatorReadyCond;

    static TilesManager* gInstance;

    ShaderProgram m_shader;
};

} // namespace WebCore

#endif // USE(ACCELERATED_COMPOSITING)
#endif // TilesManager_h
