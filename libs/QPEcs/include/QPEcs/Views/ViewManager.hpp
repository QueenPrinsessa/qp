#pragma once
#include "GenericView.hpp"
namespace QPEcs
{
	template <class ... Components>
	class View;

	class EntityComponentSystem;
	class ViewManager
	{
		using TypeName = const char*;
	public:
		ViewManager() = delete;
		explicit ViewManager(ComponentManager* aComponentManager);
		~ViewManager();

		template <class ... Components>
		inline void RegisterView(EntityComponentSystem* aECS);

		template <class ... Components>
		inline bool IsRegistered();

		inline void OnEntityDestroyed(Entity aEntity);

		inline void OnEntitySignatureChanged(Entity aEntity, Signature aEntitySignature);

		template <class ... Components>
		inline const View<Components...>* GetView();
	private:
		ComponentManager* myComponentManager { nullptr };
		std::unordered_map<TypeName, GenericView*> myViews {};
		std::unordered_map<TypeName, Signature> myViewSignatures {};

		template <class ... Components>
		inline constexpr TypeName GetTypeName();
	};

	inline ViewManager::~ViewManager()
	{
		for(auto& [typeName, viewPtr] : myViews)
		{
			delete viewPtr;
			viewPtr = nullptr;
		}
	}

	template <class ... Components>
	void ViewManager::RegisterView(EntityComponentSystem* aECS)
	{
		TypeName typeName = GetTypeName<View<Components...>>();
		auto it = myViews.find(typeName);
		if(it == myViews.end())
		{
			myViews[typeName] = new View<Components...>();
			myViews[typeName]->myECS = aECS;

			Signature signature;
			((signature.set(myComponentManager->GetComponentType<Components>())), ...);
			myViewSignatures[typeName] = signature;
		}
	}

	template <class ... Components>
	bool ViewManager::IsRegistered()
	{
		return myViews.find(GetTypeName<View<Components...>>()) != myViews.end();
	}

	template <class ... Components>
	const View<Components...>* ViewManager::GetView()
	{
		auto it = myViews.find(GetTypeName<View<Components...>>());
		if(it != myViews.end())
		{
			return static_cast<View<Components...>*>(it->second);
		}
		else
		{
			return nullptr;
		}
	}

	template <class ... Components>
	constexpr ViewManager::TypeName ViewManager::GetTypeName()
	{
		return typeid(View<Components...>).name();
	}

	inline ViewManager::ViewManager(ComponentManager* aComponentManager)
		: myComponentManager(aComponentManager)
	{
	}

	inline void ViewManager::OnEntityDestroyed(Entity aEntity)
	{
		for (auto& [typeName, view] : myViews)
		{
			view->myEntities.erase(aEntity);
		}
	}

	inline void ViewManager::OnEntitySignatureChanged(Entity aEntity, Signature aEntitySignature)
	{
		for (auto const& [typeName, view] : myViews)
		{
			const auto& viewSignature = myViewSignatures[typeName];

			if ((aEntitySignature & viewSignature) == viewSignature)
			{
				view->myEntities.insert(aEntity);
			}
			else
			{
				view->myEntities.erase(aEntity);
			}
		}
	}
}
