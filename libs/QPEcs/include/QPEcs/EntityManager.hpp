#pragma once
#include "Entity.hpp"
#include "Component.h"
#include <queue>
#include <array>
#include <cassert>

namespace QPEcs
{
	class EntityManager
	{
		friend class EntityComponentSystem;
		public:
			EntityManager();
			~EntityManager() = default;

			Entity CreateEntity();

			void DestroyEntity(Entity aEntity);

			void SetSignature(Entity aEntity, Signature aSignature);

			Signature GetSignature(Entity aEntity) const;

			bool IsValid(Entity aEntity) const;
			
		private:
			std::bitset<MaxEntities> myEntities {};
			std::array<Signature, MaxEntities> mySignatures {};
			uint32_t myEntitiesCount {};
			
	};

	inline bool EntityManager::IsValid(Entity aEntity) const
	{
		if(aEntity >= MaxEntities)
		{
			return false;
		}

		return myEntities[aEntity];
	}


	inline EntityManager::EntityManager()
	{
	}

	inline Entity EntityManager::CreateEntity()
	{
		if(myEntitiesCount < MaxEntities)
		{
			Entity entity = 0;
			for (EntityType nextEntity = 0; nextEntity < MaxEntities; nextEntity++)
			{
				if(!myEntities[nextEntity])
				{
					entity = nextEntity;
					break;
				}
			}

			myEntities.set(entity);
			myEntitiesCount++;
			return entity;
		}
		else
		{
			assert(false && "Number of entities exceeding max entities!");
			return QPEcs::NullEntity;
		}
	}

	inline void EntityManager::DestroyEntity(Entity aEntity)
	{
		if(aEntity < MaxEntities)
		{
			myEntities.reset(aEntity);
			mySignatures[aEntity].reset();

			myEntitiesCount--;
		}
	}

	inline void EntityManager::SetSignature(Entity aEntity, Signature aSignature)
	{
		if(aEntity < MaxEntities)
		{
			mySignatures[aEntity] = aSignature;
		}
	}

	inline Signature EntityManager::GetSignature(Entity aEntity) const
	{
		if(aEntity < MaxEntities)
		{
			return mySignatures[aEntity];
		}
		else
		{
			return Signature {};
		}
	}

}
