/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#ifndef AssetManager_h__
#define AssetManager_h__

#include "GfxPrerequisites.h"
#include "Singleton.h"
#include "Asset.h"

namespace Demi
{
    /** Description for an asset type
     */
    struct DiAssetDesc
    {
        // for registering new asset types
        typedef std::function<DiAssetPtr(DiDataStreamPtr,const DiString&)> AssetLoaderFunc;
        
        DiAssetType type;
        
        /// file extensions
        DiVector<DiString> extensions;
        
        /// creator callback
        AssetLoaderFunc creator;
    };
    
    /** Asset manager
     */
    class DI_GFX_API DiAssetManager : public DiSingleton<DiAssetManager>
    {
    public:

        DiAssetManager();

        virtual ~DiAssetManager();

        _DECLARE_SINGLETON(DiAssetManager)

    public:

        DiAssetPtr                    GetAsset(const DiString& path, 
                                                DiAssetType type, bool ignoreError=false);

        template <class TAsset>
        shared_ptr<TAsset>            GetAsset(const DiString& path, bool ignoreError=false)
        {
            return std::dynamic_pointer_cast<TAsset>(GetAsset(path,TAsset::TYPE,ignoreError));
        }
        

        DiAssetPtr                    CreateManualAsset(const DiString& name, 
                                                DiAssetType type);

        template <class TAsset>
        shared_ptr<TAsset>            CreateManualAsset(const DiString& name)
        {
            return std::dynamic_pointer_cast<TAsset>(CreateManualAsset(name,TAsset::TYPE));
        }

        DiAssetPtr                    CreateOrReplaceAsset(const DiString& name, 
                                                DiAssetType type);

        template <class TAsset>
        shared_ptr<TAsset>            CreateOrReplaceAsset(const DiString& name)
        {
            return std::dynamic_pointer_cast<TAsset>(CreateOrReplaceAsset(name,TAsset::TYPE));
        }

        bool                          AssetExists(const DiString& name);

        void                          DeclareAsset(const DiString& fullName);
                                      
        void                          DestroyAsset(const DiString& name);
                                      
        void                          DestroyAllAssets();

        void                          AddSearchPath(const DiString& path);

        void                          ClearSearchPaths(void);

        const DiString&               GetBasePath() const { return mBasePath; }

        void                          SetBasePath(const DiString& val);
                                      
        DiDataStreamPtr               OpenArchive(const DiString& filename, bool ignoreError=false);

        bool                          HasArchive(const DiString& filename);

        DiAssetPtr                    FindAsset(const DiString& name);

        template <class TAsset>
        shared_ptr<TAsset>            FindAsset(const DiString& name)
        {
            return std::dynamic_pointer_cast<TAsset>(FindAsset(name));
        }
        
        // for registering new asset types
        typedef std::function<DiAssetPtr(const DiString&)> AssetLoaderFunc;
        
        void                          RegisterAssetType(uint32 assetType,
                                            const DiVector<DiString>& extensions, const AssetLoaderFunc& loader);
        
        void                          RegisterAssetType(uint32 assetType,
                                                        const DiString& extension, const AssetLoaderFunc& loader);

    protected:

        DiAssetPtr                   LoadAsset(const DiString& path, bool ignoreError = false);

        DiAssetPtr                   LoadXMLAsset(DiDataStreamPtr data, const DiString& path);

        DiAssetPtr                   LoadTextureAsset(DiDataStreamPtr data, const DiString& path);

        DiAssetPtr                   LoadMeshAsset(DiDataStreamPtr data, const DiString& path);

        DiAssetPtr                   LoadMotionAsset(DiDataStreamPtr data, const DiString& path);

        DiAssetPtr                   LoadSceneAsset(DiDataStreamPtr data, const DiString& path);

    protected:

        typedef DiStrHash<DiAssetPtr> AssetsTable;
        typedef DiStrHash<ArchivePtr> ArchiveTable;

        DiVector<DiString>           mSearchPaths;

        AssetsTable                  mAssets;

        ArchiveTable                 mArchives;
                                     
        DiString                     mBasePath;

        DiArchiveManager*            mArchiveManager;
        
        typedef DiMap<uint32,AssetLoaderFunc> AssetTypeLoaders;
        typedef DiStrHash<AssetLoaderFunc>    ExtensionLoaders;
        AssetTypeLoaders             mAssetLoaders;
        
        ExtensionLoaders             mExtensionLoaders;
    };
}

#endif // AssetManager_h__