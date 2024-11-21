#pragma once

#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "Views/ViewManager.hpp"
#include <functional>

namespace QPEcs
{
	template <class ... Components>
	class View;

	class EntityComponentSystem
	{
		public:
			EntityComponentSystem();
			~EntityComponentSystem() = default;

			inline Entity CreateEntity();

			inline void DestroyEntity(Entity aEntity);

			bool IsValidEntity(Entity aEntity) const;

			template <class Component>
			inline bool HasComponent(Entity aEntity);

			template <class Component>
			inline bool IsComponentRegistered();

			template <class Component, typename ... Args>
			inline Component& AddComponent(Entity aEntity, Args&&... aArgs);

			template <class Component>
			inline void RemoveComponent(Entity aEntity);

			template <class Component>
			inline void TryCopyComponent(Entity aFrom, Entity aTo);

			template <class ... Components>
			inline void TryCopyComponents(Entity aFrom, Entity aTo);

			template <class Component>
			inline Component& GetComponent(Entity aEntity);

			template <class Component, typename ... Args>
			inline Component& GetOrAddComponent(Entity aEntity, Args&&... aArgs);

			template <class Component>
			inline ComponentType GetComponentType();

			template <class ... Components>
			inline const View<Components...>& GetView();

			inline void ForEach(std::function<void(Entity)> aFunctionToRun) const;

		private:
			std::unique_ptr<EntityManager> myEntityManager;
			std::unique_ptr<ComponentManager> myComponentManager;
			std::unique_ptr<ViewManager> myViewManager;

			void NotifyViewsOfAllEntities();

			template <class Component>
			inline void CopyComponent(Entity aFrom, Entity aTo);

			template <class ... Components>
			inline void CopyComponents(Entity aFrom, Entity aTo);
	};

	inline void EntityComponentSystem::ForEach(std::function<void(Entity)> aFunctionToRun) const
	{
		for (EntityType entity = 0; entity < MaxEntities; entity++)
		{
			if (IsValidEntity(entity))
			{
				aFunctionToRun(entity);
			}
		}
	}

	inline bool EntityComponentSystem::IsValidEntity(Entity aEntity) const
	{
		return myEntityManager->IsValid(aEntity);
	}

	template <class Component>
	inline bool EntityComponentSystem::IsComponentRegistered()
	{
		return myComponentManager->IsRegistered<Component>();
	}

	template <class Component>
	inline bool EntityComponentSystem::HasComponent(Entity aEntity)
	{
		Signature componentSignature;
		componentSignature.set(myComponentManager->GetComponentType<Component>());
		return (myEntityManager->GetSignature(aEntity) & componentSignature) == componentSignature;
	}

	template <class Component, typename ... Args>
	inline Component& EntityComponentSystem::AddComponent(Entity aEntity, Args&&... aArgs)
	{
		myComponentManager->RegisterComponent<Component>();

		myComponentManager->AddComponent<Component>(aEntity, std::forward<Args>(aArgs)...);

		auto signature = myEntityManager->GetSignature(aEntity);
		signature.set(myComponentManager->GetComponentType<Component>());
		myEntityManager->SetSignature(aEntity, signature);

		myViewManager->OnEntitySignatureChanged(aEntity, signature);

		return myComponentManager->GetComponent<Component>(aEntity);
	}

	template <class Component>
	inline void EntityComponentSystem::RemoveComponent(Entity aEntity)
	{
		if (myComponentManager->IsRegistered<Component>()
			&& HasComponent<Component>(aEntity))
		{
			myComponentManager->RemoveComponent<Component>(aEntity);

			auto signature = myEntityManager->GetSignature(aEntity);
			signature.reset(myComponentManager->GetComponentType<Component>());
			myEntityManager->SetSignature(aEntity, signature);

			myViewManager->OnEntitySignatureChanged(aEntity, signature);
		}
	}

	template <class Component>
	void EntityComponentSystem::CopyComponent(Entity aFrom, Entity aTo)
	{
		if (myComponentManager->IsRegistered<Component>())
		{
			myComponentManager->CopyComponent<Component>(aFrom, aTo);

			auto signature = myEntityManager->GetSignature(aTo);
			signature.set(myComponentManager->GetComponentType<Component>());
			myEntityManager->SetSignature(aTo, signature);

			myViewManager->OnEntitySignatureChanged(aTo, signature);
		}
	}

	template <class ... Components>
	void EntityComponentSystem::CopyComponents(Entity aFrom, Entity aTo)
	{
		((CopyComponent<Components>(aFrom, aTo)), ...);
	}

	template <class Component>
	void EntityComponentSystem::TryCopyComponent(Entity aFrom, Entity aTo)
	{
		if (!HasComponent<Component>(aFrom))
		{
			return;
		}

		CopyComponent<Component>(aFrom, aTo);
	}

	template <class ... Components>
	void EntityComponentSystem::TryCopyComponents(Entity aFrom, Entity aTo)
	{
		((TryCopyComponent<Components>(aFrom, aTo)), ...);
	}

	template <class Component>
	inline Component& EntityComponentSystem::GetComponent(Entity aEntity)
	{
		return myComponentManager->GetComponent<Component>(aEntity);
	}

	template <class Component, typename ... Args>
	inline Component& EntityComponentSystem::GetOrAddComponent(Entity aEntity, Args&&... aArgs)
	{
		if (!HasComponent<Component>(aEntity))
		{
			return AddComponent<Component>(aEntity, std::forward<Args>(aArgs)...);
		}

		return GetComponent<Component>(aEntity);
	}

	template <class Component>
	inline ComponentType EntityComponentSystem::GetComponentType()
	{
		return myComponentManager->GetComponentType<Component>();
	}

	template <class ... Components>
	const View<Components...>& EntityComponentSystem::GetView()
	{
		auto viewPtr = myViewManager->GetView<Components...>();
		if (!viewPtr)
		{
			myViewManager->RegisterView<Components...>(this);
			NotifyViewsOfAllEntities();
			return *myViewManager->GetView<Components...>();
		}
		else
		{
			return *viewPtr;
		}
	}

	inline EntityComponentSystem::EntityComponentSystem()
	{
		myComponentManager = std::make_unique<ComponentManager>();
		myEntityManager = std::make_unique<EntityManager>();
		myViewManager = std::make_unique<ViewManager>(myComponentManager.get());
	}

	inline Entity EntityComponentSystem::CreateEntity()
	{
		return myEntityManager->CreateEntity();
	}

	inline void EntityComponentSystem::DestroyEntity(Entity aEntity)
	{
		if (IsValidEntity(aEntity))
		{
			myEntityManager->DestroyEntity(aEntity);
			myComponentManager->OnEntityDestroyed(aEntity);
			myViewManager->OnEntityDestroyed(aEntity);
		}
	}

	inline void EntityComponentSystem::NotifyViewsOfAllEntities()
	{
		for (EntityType entity = 0; entity < MaxEntities; entity++)
		{
			if (myEntityManager->myEntities[entity])
			{
				myViewManager->OnEntitySignatureChanged(entity, myEntityManager->GetSignature(entity));
			}
		}
	}
}
