// Copyright 2024 Daniel Karasov Lerner. All Rights Reserved.

#include "EditorFont.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include <Widgets/Layout/SBox.h>

#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Misc/OutputDeviceNull.h"



#define LOCTEXT_NAMESPACE "FEditorFontModule"

void FEditorFontModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	//ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	//if (SettingModule)
	//{	
	//	auto setting = SettingModule->RegisterSettings("Editor", "EditorFont", "Setting",
	//												   LOCTEXT("RuntimeSettingsName", "EditorFont Settings"),
	//												   LOCTEXT("RuntimeSettingsDescription", "Configure my setting"),
	//												   GetMutableDefault<USetting>()
	//	);

	//	
	//	if (setting.IsValid())
	//	{
	//		setting->OnResetDefaults().BindUFunction(GetMutableDefault<USetting>(), FName("OnResetDefaults"));
	//	}

	//}
	
	MyRegisterSettings();
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	EditMenuExtender = MakeShareable(new FExtender());
	EditMenuExtender->AddMenuExtension(	"EditMain",EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateRaw(this, &FEditorFontModule::AddEditMenuExtension));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(EditMenuExtender);
}


void FEditorFontModule::MyRegisterSettings()
{
	
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");	
	PropertyModule.RegisterCustomClassLayout("DevEditor", FOnGetDetailCustomizationInstance::CreateStatic(&FEFDetails::MakeInstance));
}

void FEditorFontModule::MyUnregisterSettings()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout("DevEditor");

	// De-Initialize my slate style.
	//FEFSlateStyle::Shutdown();
}

void FEditorFontModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	//{
	//	SettingsModule->UnregisterSettings("Editor", "Plugins", "Setting");
	//}
	MyUnregisterSettings();
}


void FEditorFontModule::OpenEditorPreferences()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->ShowViewer("Editor", "EditorFont", "DevEditor");
	}
}

void FEditorFontModule::AddEditMenuExtension(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FText::FromString("EditorFont Menu"), FText::FromString("Open the settings for EditorFont"), FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &FEditorFontModule::OpenEditorPreferences)));
}

FReply FEditorFontModule::OnNewButtonClicked() const
{
	UDevEditor* Settings = GetMutableDefault<UDevEditor>();

	//Save all open tabs
	FGlobalTabmanager::Get()->SaveAllVisualState();

	//Flush our config files
	const bool bRead = true;
	GConfig->Flush(bRead, GEditorLayoutIni);

	//Request the editor to rebuild Slate
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameModule.RecreateDefaultMainFrame(false, false);
	return FReply::Handled();
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorFontModule, EditorFont)




//=====================================
//===========**FEFDetails**============
//=====================================

#define LOCTEXT_NAMESPACE "FEFDetails"



void FEFDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	TArray<TWeakObjectPtr<UObject>> Outers;
	DetailBuilder.GetObjectsBeingCustomized(Outers);
	if (Outers.Num() == 0) return;
	auto MyDevSettings = Cast<UDevEditor>(Outers[0].Get());

	TSharedPtr<SCheckBox> ToggleCheckbox;

	/// Reset to defaults button
	TSharedRef<IPropertyHandle> ToggleEdit = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDevEditor, ToggleSettingsEditable));
	DetailBuilder.EditCategory("Settings")
		.AddProperty(ToggleEdit)
		.CustomWidget()
		.NameContent()
		[
			ToggleEdit->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				//.FillWidth(1.0f)
				[
					ToggleEdit->CreatePropertyValueWidget()
				]
		]
		.ExtensionContent()
		[

			SNew(SButton)
				.Text(FText::FromString("Reset to Defaults"))
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.ButtonColorAndOpacity((FLinearColor(0.92f, 0.3f, 0.34f, 1.0)))
				.OnClicked_Lambda([this, MyDevSettings, &DetailBuilder]()
					{
						//UE_LOG(LogTemp, Error, TEXT("Defaults reinstated."));
						MyDevSettings->MyOnResetDefaults();
						return FReply::Handled();
					})

		];


	/// Reset to defaults button for Font Path
	TSharedRef<IPropertyHandle> FontsPath = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDevEditor, FontPath));
	FName PropertyName = FontsPath->GetProperty()->GetFName();
	// Initialize the state if it doesn't exist
	if (!MyDevSettings->ToggleStates.Contains(PropertyName))
	{
		MyDevSettings->ToggleStates.Add(PropertyName, "True");
	}
	bool bIsEditable = true;
	DetailBuilder.EditCategory("Settings")
		.AddProperty(FontsPath)
		.CustomWidget()
		.NameContent()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				[
					SAssignNew(ToggleCheckbox, SCheckBox)
						.ToolTipText(INVTEXT("Lock this individual setting."))
						.OnCheckStateChanged_Lambda([MyDevSettings, PropertyName, &DetailBuilder](ECheckBoxState NewState)
							{
								if (NewState == ECheckBoxState::Checked)
								{
									MyDevSettings->ToggleStates[PropertyName] = "True";
									MyDevSettings->SaveSettingToConfig();
								}
								else
								{
									MyDevSettings->ToggleStates[PropertyName] = "False";
									MyDevSettings->SaveSettingToConfig();
								}
							})
						.IsChecked_Lambda([MyDevSettings, PropertyName]() -> ECheckBoxState
							{
								return MyDevSettings->ToggleStates[PropertyName] == "True" ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
							})

				]
				+ SHorizontalBox::Slot()
				[
					FontsPath->CreatePropertyNameWidget()
				]
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(SBox)
						.IsEnabled_Lambda([MyDevSettings, PropertyName]() {
						return MyDevSettings->ToggleStates[PropertyName] == "True";
							})

						[
							FontsPath->CreatePropertyValueWidget()
						]

				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
						.IsEnabled_Lambda([MyDevSettings, PropertyName]() {
						return MyDevSettings->ToggleStates[PropertyName] == "True";
							})
						.HAlign(EHorizontalAlignment::HAlign_Right)
						[
							SNew(SButton)
								.Text(FText::FromString("::::"))
								.ButtonColorAndOpacity(FLinearColor(0.98f, 0.72f, 0.01f, 1.0))
								.Cursor(EMouseCursor::Hand)
								.HAlign(EHorizontalAlignment::HAlign_Left)
								.OnClicked_Lambda([MyDevSettings, FontsPath]()
									{
										FString blagh = MyDevSettings->FontPath;
										FString bleh = MyDevSettings->FilePicker(FontsPath->GetProperty()->GetName()/"Picker");
										if (bleh == FString(""))
										{
											bleh = blagh;
										}
										MyDevSettings->FontPath = bleh;
										MyDevSettings->SaveSettingToConfig();
										return FReply::Handled();
									})
						]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(FMargin(10.0f, 0.0f, 10.0f, 0.0f))
				[
					SNew(SBox)
						.IsEnabled_Lambda([MyDevSettings, PropertyName]() {
						return MyDevSettings->ToggleStates[PropertyName] == "True";
							})
						.HAlign(EHorizontalAlignment::HAlign_Right)
						[
							SNew(SButton)
								.Text(FText::FromString("Reset Path"))
								.HAlign(EHorizontalAlignment::HAlign_Right)
								.OnClicked_Lambda([MyDevSettings]()
									{
										MyDevSettings->FontPath = MyDevSettings->PluginContentPath + "/TempFonts";
										MyDevSettings->SaveSettingToConfig();
										MyDevSettings->CreateTempFontsFolder();
										return FReply::Handled();
									})
						]
				]
		];

	/// Directory picker for folder-iterating font conversion
	TSharedRef<IPropertyHandle> DirConverter = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDevEditor, FontChanger));
	FName DirConvName = DirConverter->GetProperty()->GetFName();
	TSharedPtr<SWidget> NameDefault;
	TSharedPtr<SWidget> ValueDefault;
	DetailBuilder.EditDefaultProperty(DirConverter)->GetDefaultWidgets(NameDefault, ValueDefault);
	DetailBuilder.EditCategory("Settings")
		.AddProperty(DirConverter)
		
		.CustomWidget()
		.NameContent()
		[
			DirConverter->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.MinWidth(200)
			[
				ValueDefault.ToSharedRef()
			]
			
			+ SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                	.HAlign(EHorizontalAlignment::HAlign_Right)
                	[
                		SNew(SButton)
                			.Text(FText::FromString("Folder Conversion"))
                			.ButtonColorAndOpacity(FLinearColor(0.88f, 0.62f, 0.01f, 1.0))
                			.Cursor(EMouseCursor::Hand)
                			.HAlign(EHorizontalAlignment::HAlign_Left)
                			.ToolTipText(FText::FromString("Choose a valid directory. All .ttf and .otf fonts will be converted to the other type."))
                			.OnClicked_Lambda([MyDevSettings, DirConverter]()
                				{
                					FString bloop = MyDevSettings->FilePicker("Font Conversion Directory Picker");
                					if (bloop.IsEmpty())
                					{
                						return FReply::Handled();
                					}
                					MyDevSettings->ConvertFontFolder(DirConverter->GetProperty(), bloop);
                					return FReply::Handled();
                				})
					]
            ]
		];
		


	/// Per-property reset button
	TArray<TSharedRef<IPropertyHandle>> fontProperties;
	FText SampleText = LOCTEXT("EditorFontText_Default", "Everyone has the right to freedom of font.");
	
	DetailBuilder.EditCategory("Fonts").GetDefaultProperties(fontProperties);

	for (TSharedRef<IPropertyHandle> propHandle : fontProperties)
	{
		if (!MyDevSettings->ToggleStates.Contains(propHandle->GetProperty()->GetFName()))
		{
			MyDevSettings->ToggleStates.Add(propHandle->GetProperty()->GetFName(), "True");
		}
		FName filename = FName("FileName");
		FString PropFileName = propHandle->GetMetaData(filename);
		const bool bIsArabicFont = (PropFileName == "NotoNaskhArabicUI-Regular.ttf");
		const bool bIsThaiFont = (PropFileName == "NotoSansThai-Regular.ttf");
		const bool bIsJapaneseFont = (PropFileName == "GenEiGothicPro-Bold.otf"||PropFileName == "GenEiGothicPro-Regular.otf"||PropFileName=="GenEiGothicPro-Heavy.otf"||PropFileName=="GenEiGothicPro-Light.otf"||PropFileName=="GenEiGothicPro-Semibold.otf");
		const bool bIsKorean = (PropFileName == "NanumGothic.ttf"||PropFileName == "NanumGothicBold.ttf"||PropFileName == "NanumGothicExtraBold.ttf");
		
		if (bIsArabicFont){SampleText=LOCTEXT("EditorFontText_Arabic", "لكل فرد الحق في حرية الخط.");}
		if (bIsThaiFont){SampleText=LOCTEXT("EditorFontText_Thai", "ทุกคนมีสิทธิในเสรีภาพในการใช้แบบอักษร");}
		if (bIsJapaneseFont){SampleText=LOCTEXT("EditorFontText_JP", "誰もがフォントの自由に対する権利を有します。");}
		if (bIsKorean){SampleText=LOCTEXT("EditorFontText_KO", "모든 사람은 글꼴의 자유를 누릴 권리가 있습니다.");}
		FString DestFontPath = propHandle->GetBoolMetaData("AlternateFont") ? FPaths::EngineContentDir() / TEXT("Editor/Slate/Fonts/") : FPaths::EngineContentDir() / TEXT("Slate/Fonts/");
		DetailBuilder.EditCategory("Fonts").AddProperty(propHandle)
			.CustomWidget()
			.NameContent()
			.MaxDesiredWidth(150)
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Right)
					[
						SAssignNew(ToggleCheckbox, SCheckBox)
							.ToolTipText(INVTEXT("Lock this individual setting."))
							.OnCheckStateChanged_Lambda([MyDevSettings, propHandle, &DetailBuilder](ECheckBoxState NewState)
								{
									if (NewState == ECheckBoxState::Checked)
									{
										MyDevSettings->ToggleStates[propHandle->GetProperty()->GetFName()] = "True";
										MyDevSettings->SaveSettingToConfig();
									}
									else
									{
										MyDevSettings->ToggleStates[propHandle->GetProperty()->GetFName()] = "False";
										MyDevSettings->SaveSettingToConfig();
									}
								})
							.IsChecked_Lambda([MyDevSettings, propHandle]() -> ECheckBoxState
								{
									return MyDevSettings->ToggleStates[propHandle->GetProperty()->GetFName()] == "True" ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})

					]
					+ SHorizontalBox::Slot()
					[
						propHandle->CreatePropertyNameWidget()
					]
			]
			.ValueContent()
			.MinDesiredWidth(300)
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SBox)
							.IsEnabled_Lambda([MyDevSettings, propHandle]() {
							return MyDevSettings->ToggleStates[propHandle->GetProperty()->GetFName()] == "True";
							
							})
							[
								propHandle->CreatePropertyValueWidget()
							]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(FMargin(10.0f, 0.0f, 10.0f, 0.0f))
					[
						SNew(SBox)
							.IsEnabled_Lambda([MyDevSettings, propHandle]() {
							return MyDevSettings->ToggleStates[propHandle->GetProperty()->GetFName()] == "True";

							})
							[
							SNew(SButton)
								.Text(FText::FromString("Reset"))
								.ToolTipText(INVTEXT("Reset this individual font property."))
								.OnClicked_Lambda([MyDevSettings, propHandle, &DetailBuilder]()
												  {
													  //if(propHandle == DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UDevEditor, FontPath))){ return FReply::Handled(); }
													  UE_LOG(LogTemp, Error, TEXT("Property reset"));
													  MyDevSettings->ResetToDefaults(propHandle->GetProperty());
													  return FReply::Handled();
												  })
							]
					]
			]
			.ExtensionContent()
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(STextBlock)
							.Text(SampleText)
							.Justification(ETextJustify::InvariantLeft)
							.TextFlowDirection(bIsArabicFont ? ETextFlowDirection::RightToLeft : ETextFlowDirection::LeftToRight)
							.Margin(FMargin(20.0f, 0.0f, 0.0f, 0.0f))
							.Font(FSlateFontInfo(DestFontPath / PropFileName, 8))
					]
			];
	}


	/// Update fonts button
	DetailBuilder.EditCategory("Fonts")
		.AddCustomRow(FText::FromString("Fonts"))
		.NameContent()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
						.Text(FText::FromString("Update the fonts in your editor."))
						.Justification(ETextJustify::Left)
				]
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[

					SNew(SButton)
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.Text(FText::FromString("Update Fonts"))
						.ToolTipText(INVTEXT("Will update the fonts in the editor."))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.ButtonColorAndOpacity((FLinearColor(0.5f, 0.939f, 0.524f, 1.0)))
						.OnClicked_Lambda([]()
								{
								FSlateApplication::Get().GetRenderer()->FlushFontCache(TEXT("Fonts have been updated."));

								return FReply::Handled();
								})
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(10.0f, 0.0f, 10.0f, 0.0f))
				.AutoWidth()
				[

					SNew(SButton)
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.Text(FText::FromString("Refresh Windows"))
						.ToolTipText(INVTEXT("Will refresh all window panels"))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.ButtonColorAndOpacity((FLinearColor::Blue))
						.OnClicked_Lambda([&DetailBuilder]()
										  {

											  //TSharedRef<ISlateStyle> Style = FMyStyle::Create();
											  //FSlateStyleRegistry::UnRegisterSlateStyle(Style.Get());
											  //FSlateStyleRegistry::RegisterSlateStyle(Style.Get());

											  ///*Refresh windows*/
											  DetailBuilder.EditCategory("Fonts").GetParentLayout().ForceRefreshDetails();
											  FGlobalTabmanager::Get()->SaveAllVisualState();
											  GConfig->Flush(true, GEditorLayoutIni);
											  IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
											  MainFrameModule.RecreateDefaultMainFrame(false, false);
											  return FReply::Handled();
										  })
				]
		];
	MyDevSettings->LoadConfigFile();
	}

	void FEFDetails::RefreshDetails(IDetailLayoutBuilder* DetailBuilder)
	{
		DetailBuilder->ForceRefreshDetails();
	}

	bool FEFDetails::GetCheckBoxState(FName PropertyName) const
	{
		const bool* State = CheckBoxStates.Find(PropertyName);
		return State ? *State : false;
	}

	void FEFDetails::SetCheckBoxState(FName PropertyName, bool bState)
	{
		CheckBoxStates.Add(PropertyName, bState);
	}





#undef LOCTEXT_NAMESPACE