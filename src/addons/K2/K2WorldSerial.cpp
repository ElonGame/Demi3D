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

#include "K2Pch.h"
#include "K2WorldSerial.h"
#include "K2World.h"
#include "K2MapLoader.h"
#include "K2Configs.h"
#include "K2Model.h"
#include "K2Terrain.h"
#include "K2TerrainDesc.h"
#include "PathLib.h"

#include "XMLFile.h"
#include "XMLElement.h"
#include "CullNode.h"

namespace Demi
{
    void DiK2WorldSerial::Load(const DiString& path, DiK2World* world)
    {
        DI_LOG("Loading k2 world: %s", path.c_str());
        
        if(!DiK2Configs::K2ArchiveExists(path, false))
        {
            DI_WARNING("Cannot locate the k2 map: %s", path.c_str());
            return;
        }

        DiTerrainDescPtr terrainDesc = make_shared<DiTerrainDesc>();

        terrainDesc->mHeightMap = DI_NEW DiK2HeightMap();
        terrainDesc->mHeightMap->Load(DiK2Configs::GetDataStream(path + "/heightmap", K2_RES_BINARY));

        terrainDesc->mTextureIDMap = DI_NEW DiK2TileMap();
        terrainDesc->mTextureIDMap->Load(DiK2Configs::GetDataStream(path + "/tilematerialmap", K2_RES_BINARY));

        terrainDesc->mColorMap = DI_NEW DiK2VertexColorMap();
        terrainDesc->mColorMap->Load(DiK2Configs::GetDataStream(path + "/vertexcolormap", K2_RES_BINARY));

        terrainDesc->mTileCliffMap = DI_NEW DiK2TileCliffMap();
        terrainDesc->mTileCliffMap->Load(DiK2Configs::GetDataStream(path + "/tilecliffmap", K2_RES_BINARY));

        terrainDesc->mVertBlockerMap = DI_NEW DiK2VertexBlockerMap();
        terrainDesc->mVertBlockerMap->Load(DiK2Configs::GetDataStream(path + "/vertexblockermap", K2_RES_BINARY));

        // load texture list
        LoadTextureList(path, terrainDesc);

        terrainDesc->mSizeX = (terrainDesc->mHeightMap->GetWidth() - 1) / CHUNK_GRID_SIZE;
        terrainDesc->mSizeY = (terrainDesc->mHeightMap->GetHeight() - 1) / CHUNK_GRID_SIZE;

        LoadWorldConfig(path, terrainDesc, world);

        world->mTerrain = make_shared<DiTerrain>();
        world->mTerrain->Load(terrainDesc);

        // load entities
        LoadEntityList(path, world);
    }

    void DiK2WorldSerial::LoadTextureList(const DiString& path, DiTerrainDescPtr terrainDesc)
    {
        DiDataStreamPtr texListData = DiK2Configs::GetDataStream(path + "/texturelist", K2_RES_XML);

        shared_ptr<DiXMLFile> xmlfile(DI_NEW DiXMLFile());
        xmlfile->Load(texListData->GetAsString());

        DiXMLElement root = xmlfile->GetRoot();
        if (root.GetName() != "texturelist")
        {
            DI_WARNING("Invalid K2 texturelist script");
            return;
        }

        terrainDesc->mTextureTable.clear();
        DiXMLElement child = root.GetChild();
        while (child)
        {
            if (child.CheckName("texture"))
            {
                int id = child.GetInt("id");
                DiString name = child.GetAttribute("name");

                DI_LOG("K2 terrain tile[%d]: %s", id, name.c_str());

                if (name == "$checker" || name == "$flat_dull")
                    name.clear(); // dummy texture

                name.TrimLeft("/");
                name = name.ExtractBaseName();

                terrainDesc->mTextureTable[id] = name;
            }
            child = child.GetNext();
        }
    }

    void DiK2WorldSerial::LoadEntityList(const DiString& path, DiK2World* world)
    {
        DiDataStreamPtr texListData = DiK2Configs::GetDataStream(path + "/entitylist", K2_RES_XML);

        DI_LOG("Loading K2 entity lists");

        shared_ptr<DiXMLFile> xmlfile(DI_NEW DiXMLFile());
        xmlfile->Load(texListData->GetAsString());

        DiXMLElement root = xmlfile->GetRoot();
        if (root.GetName() != "entitylist")
        {
            DI_WARNING("Invalid K2 entitylist script");
            return;
        }

        DiXMLElement child = root.GetChild();
        while (child)
        {
            if (child.CheckName("entity"))
            {
                int id = child.GetInt("index");
                int team = child.GetInt("team");

                DiString model = child.GetAttribute("model");
                model.TrimLeft("/");
                model.ToLower();
                DiString type = child.GetAttribute("type");

                DiVec3 angles = child.GetVector3("angles");
                std::swap(angles.y, angles.z);

                Trans transform;

                transform.rot = DiK2Configs::ConvertAngles(angles);
                transform.pos = child.GetVector3("position");
                std::swap(transform.pos.y, transform.pos.z);

                DiVec2 worldSize = world->mTerrain->GetWorldSize();
                transform.pos.x = worldSize.x - transform.pos.x;

                float scale = child.GetFloat("scale");
                transform.scale = DiVec3(scale, scale, scale);

                world->ProcessWorldEntity(model, type, transform, id, team);

            }
            child = child.GetNext();
        }
    }

    void DiK2WorldSerial::LoadWorldConfig(const DiString& path, DiTerrainDescPtr terrainDesc, DiK2World* world)
    {
        DiDataStreamPtr texListData = DiK2Configs::GetDataStream(path + "/worldconfig", K2_RES_XML);

        DI_LOG("Loading K2 worldconfig");

        shared_ptr<DiXMLFile> xmlfile(DI_NEW DiXMLFile());
        xmlfile->Load(texListData->GetAsString());

        DiXMLElement root = xmlfile->GetRoot();
        if (root.GetName() != "world")
        {
            DI_WARNING("Invalid K2 worldconfig script");
            return;
        }

        world->mName = root.GetAttribute("name");
        terrainDesc->mGridSize = root.GetFloat("scale");
        terrainDesc->mTextureScale = root.GetFloat("texturescale");
        terrainDesc->mCliffSize = root.GetUint("cliffsize");
    }
}