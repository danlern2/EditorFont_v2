// Copyright 2024 Daniel Karasov Lerner. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "DetailCustomizations.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"

#include "UDevEditor.h"
#include "DetailWidgetRow.h"

#include "Interfaces/IMainFrameModule.h"
#include "Modules/ModuleManager.h"
#include <unordered_map>

class FEditorFontModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	void MyRegisterSettings();
	void MyUnregisterSettings();
	virtual void ShutdownModule() override;


	void OpenEditorPreferences();
	
	void AddEditMenuExtension(FMenuBuilder& MenuBuilder);
	
	FReply OnNewButtonClicked() const;
private:
	TSharedPtr<FExtender> EditMenuExtender;
	
};

class FEFDetails : public IDetailCustomization
{

public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShareable(new FEFDetails()); }

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	

private:
	TSharedPtr<SVerticalBox> Test;

	TMap<FName, bool> CheckBoxStates;

	bool GetCheckBoxState(FName PropertyName) const;
	void SetCheckBoxState(FName PropertyName, bool bState);

	void RefreshDetails(IDetailLayoutBuilder* DetailBuilder);

	

};