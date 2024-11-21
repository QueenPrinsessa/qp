#pragma once
#include "Entity.hpp"
#include "Types.h"
#include "ComponentRegistry.hpp"
#include <memory>
#include <unordered_map>

namespace QPEcs
{
	class ComponentManager
	{
		using TypeName = const char*;
		public:
			~ComponentManager();

			template <class Component>
			void RegisterComponent();

			template <class Component>
			ComponentType GetComponentType();

			template <class Component, class ... Args>
			void AddComponent(Entity aEntity, Args&&... aArgs);

			template <class Component>
			void RemoveComponent(Entity aEntity);

			template <class Component>
			Component& GetComponent(Entity aEntity);

			template <class Component>
			inline void CopyComponent(Entity aFrom, Entity aTo);

			template <class Component>
			bool IsRegistered();

			void OnEntityDestroyed(Entity aEntity);

		private:
			std::unordered_map<TypeName, ComponentType> myComponentTypes {};
			std::unordered_map<TypeName, ComponentRegistryBase*> myComponentRegistries {};
			ComponentType myNextComponentType{};

			template <class Component>
			ComponentRegistry<Component>* GetComponentRegistry();

			template <class Component>
			constexpr TypeName GetTypeName();
	};

	inline ComponentManager::~ComponentManager()
	{
		for(auto& [type, registryPtr] : myComponentRegistries)
		{
			delete registryPtr;
			registryPtr = nullptr;
		}
	}

	template <class Component>
	inline bool ComponentManager::IsRegistered()
	{
		return myComponentTypes.find(GetTypeName<Component>()) != myComponentTypes.end();
	}

	inline void ComponentManager::OnEntityDestroyed(Entity aEntity)
	{
		for (auto const& [typeName, component] : myComponentRegistries)
		{
			component->OnEntityDestroyed(aEntity);
		}
	}

	template <class Component>
	void ComponentManager::RegisterComponent()
	{
		TypeName typeName = GetTypeName<Component>();
		if (myComponentTypes.find(typeName) == myComponentTypes.end())
		{
			myComponentTypes[typeName] = myNextComponentType++;
			myComponentRegistries[typeName] = new ComponentRegistry<Component>();
		}
	}

	template <class Component>
	ComponentType ComponentManager::GetComponentType()
	{
		RegisterComponent<Component>();
		return myComponentTypes[GetTypeName<Component>()];
	}

	template <class Component, class ... Args>
	void ComponentManager::AddComponent(Entity aEntity, Args&&... aArgs)
	{
		GetComponentRegistry<Component>()->AddComponent(aEntity, std::forward<Args>(aArgs)...);
	}

	template <class Component>
	void ComponentManager::RemoveComponent(Entity aEntity)
	{
		GetComponentRegistry<Component>()->RemoveComponent(aEntity);
	}

	template <class Component>
	Component& ComponentManager::GetComponent(Entity aEntity)
	{
		return GetComponentRegistry<Component>()->GetComponent(aEntity);
	}


	template <class Component>
	void ComponentManager::CopyComponent(Entity aFrom, Entity aTo)
	{
		if(myComponentTypes.find(GetTypeName<Component>()) != myComponentTypes.end())
		{
			GetComponentRegistry<Component>()->CopyComponent(aFrom, aTo);
		}
	}

	template <class Component>
	ComponentRegistry<Component>* ComponentManager::GetComponentRegistry()
	{
		auto it = myComponentRegistries.find(GetTypeName<Component>());

		if(it != myComponentRegistries.end())
		{
			return static_cast<ComponentRegistry<Component>*>(it->second);
		}
		else
		{
			return nullptr;
		}
	}

	template <class Component>
	constexpr ComponentManager::TypeName ComponentManager::GetTypeName()
	{
		return typeid(Component).name();
	}
}
