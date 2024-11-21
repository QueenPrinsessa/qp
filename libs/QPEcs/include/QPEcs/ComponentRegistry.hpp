#pragma once
#include "ComponentRegistryBase.h"
#include <array>
#include <cassert>
#include <unordered_map>
#include "Structures/SparseSet.hpp"
namespace QPEcs
{
	template <typename Component>
	class ComponentRegistry : public ComponentRegistryBase
	{
		public:
			ComponentRegistry();
			virtual ~ComponentRegistry() override;

			template<typename ... Args>
			void AddComponent(Entity aEntity, Args&&... aArgs);

			void RemoveComponent(Entity aEntity);

			Component& GetComponent(Entity aEntity);

			virtual void OnEntityDestroyed(Entity aEntity) override;

		private:
			std::array<Component, MaxEntities> myComponents {};
			SparseSet<int64_t> myEntityIndexSet { MaxEntities, MaxEntities };
	};

	template <typename Component>
	ComponentRegistry<Component>::ComponentRegistry()
	{
	}

	template <typename Component>
	ComponentRegistry<Component>::~ComponentRegistry()
	{
	}

	template <typename Component>
	template <typename ... Args>
	void ComponentRegistry<Component>::AddComponent(Entity aEntity, Args&&... aArgs)
	{
		if(!myEntityIndexSet.Contains(aEntity))
		{
			myEntityIndexSet.Insert(aEntity);

			int64_t index = myEntityIndexSet.Search(aEntity);
			myComponents[index] = Component(std::forward<Args>(aArgs)...);
		}
	}

	template <typename Component>
	void ComponentRegistry<Component>::RemoveComponent(Entity aEntity)
	{
		if(myEntityIndexSet.Contains(aEntity))
		{
			int64_t removedEntityIndex = myEntityIndexSet.Search(aEntity);
			myEntityIndexSet.Erase(aEntity);
			myComponents[removedEntityIndex] = myComponents[myEntityIndexSet.Size()];
		}
	}
	
	template <typename Component>
	Component& ComponentRegistry<Component>::GetComponent(Entity aEntity)
	{
		assert(myEntityIndexSet.Contains(aEntity) && "Entity does not have component.");
		return myComponents[myEntityIndexSet.Search(aEntity)];
	}

	template <typename Component>
	void ComponentRegistry<Component>::OnEntityDestroyed(Entity aEntity)
	{
		if (myEntityIndexSet.Contains(aEntity))
		{
			RemoveComponent(aEntity);
		}
	}
}
