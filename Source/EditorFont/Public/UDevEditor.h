// Copyright 2024 Daniel Karasov Lerner. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "CoreMinimal.h"
#include "DesktopPlatformModule.h"
//#include "Interfaces/IPluginManager.h"
//#include "EditorFont.h"
//#include "PropertyEditorDelegates.h"

#include "UDevEditor.generated.h"
#if WITH_EDITOR

UCLASS(Config = "EditorSettings", meta = (DisplayName = "Editor Font Settings"))
class EDITORFONT_API UDevEditor : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UDevEditor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	~UDevEditor();

	static UDevEditor* Get() { return CastChecked<UDevEditor>(UDevEditor::StaticClass()->GetDefaultObject()); }

	virtual TSharedPtr<SWidget> GetCustomSettingsWidget() const override;


	UPROPERTY()
	FString PluginContentPath;

	UPROPERTY(Config, EditAnywhere, Category = Settings, meta = (EditCondition = "ToggleSettingsEditable"))
	FString FontPath;
	

	UPROPERTY()
	FString SlateFontPath;
	UPROPERTY()
	FString AltSlateFontPath;

	UPROPERTY()
	FString Defaults;

	/*===========================
	* ======Font Properties======
	============================*/
	
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-Black.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayPriority = "-1"))
	FName BlackFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-BlackItalic.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont"))
	FName BlackItalicFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-Bold.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont", Tooltip = "Font used for blueprint titles."))
	FName BoldFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-BoldCondensed.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont", Tooltip = "Row widget titles"))
	FName BoldCondensedFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-BoldCondensedItalic.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont"))
	FName BoldCondensedItalicFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-BoldItalic.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont"))
	FName BoldItalicFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-Italic.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont", Tooltip = "Font used under the blueprint titles."))
	FName ItalicFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-Light.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont"))
	FName LightFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-Medium.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont"))
	FName MediumFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "Roboto-Regular.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", DisplayAfter = "BlackFont", Tooltip = "Font used for most things in the editor."))
	FName RegularFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "DroidSansMono.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable", Tooltip = "Used for the output log and viewport log, I suggest only using a mono font for it."))
	FName MonoFont;
	/*===========================
	* ======Font Properties======
	============================*/

	/*===========================
	* ========Other Fonts========
	============================*/
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "NotoNaskhArabicUI-Regular.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides="bEditLocalizedFonts", Tooltip = "The Arabic localized font."))
	FName ArabicFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = false, FileName = "NotoSansThai-Regular.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides, Tooltip = "The Thai localized font."))
	FName ThaiFont;
	/*TODO:Reenable these later when you have it ready*/
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "GenEiGothicPro-Regular.otf", GetOptions = "GetOtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides, Tooltip = "Japanese localized font used for most things in the editor."))
	FName JapaneseRegularFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "GenEiGothicPro-Bold.otf", GetOptions = "GetOtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides, Tooltip = "Japanese localized font used for bold things in the editor."))
	FName JapaneseBoldFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "GenEiGothicPro-SemiBold.otf", GetOptions = "GetOtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides))
	FName JapaneseSemiBoldFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "GenEiGothicPro-Heavy.otf", GetOptions = "GetOtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides))
	FName JapaneseHeavyFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "GenEiGothicPro-Light.otf", GetOptions = "GetOtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides))
	FName JapaneseLightFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "NanumGothic.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides, Tooltip = "Korean localized font used for most things in the editor."))
	FName KoreanRegularFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "NanumGothicBold.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides, Tooltip = "Korean localized font used for bold things in the editor."))
	FName KoreanBoldFont;
	UPROPERTY(Config, EditAnywhere, Category = Fonts, meta = (AlternateFont = true, FileName = "NanumGothicExtraBold.ttf", GetOptions = "GetTtfFonts", ShowInnerProperties, EditCondition = "ToggleSettingsEditable && bEditLocalizedFonts", EditConditionHides))
	FName KoreanBlackFont;

	UPROPERTY(Config, Category = Settings, EditAnywhere, meta = (DisplayPriority = 3, ToolTip="Show and allow editing of available foreign alphabets."))
	bool bEditLocalizedFonts{false};

	/*===========================
	* ========Other Fonts========
	============================*/

	UPROPERTY(Category = Settings, VisibleAnywhere, meta = (DisplayPriority = 1, Multiline))
	FString Instructions = "This plugin comes with a 'TempFonts' folder inside the Plugin's Content directory as the default location. Move any '.ttf' or '.otf' fonts you wish to select from to there.\nThe normal 'Reset to Default' does not work. Use the one I provide!\nFurther info in the content folder README\nDiscussion Forum Thread: https://forums.unrealengine.com/t/editorfont-discussion-thread/2312529?u=xtheos";


	UPROPERTY(Config, Category = Settings, EditAnywhere, meta = (DisplayPriority = 3, Tooltip = "Locks all settings to prevent changes."))
	bool ToggleSettingsEditable{ false };

	UPROPERTY(EditAnywhere, Category = Settings, meta=(FilePathFilter = "Font Files (*.ttf, *.otf)|*.ttf;*.otf", RelativeToGameDir, PropName = "FontChanger", DisplayPriority = 2, ToolTip="Enter a valid filepath, or use the mini filepicker"), DisplayName="Font Converter")
	FFilePath FontChanger;
	

	UFUNCTION()
	FString FilePicker(const FString& DialogueTitle);

	UFUNCTION()
	bool GetFontsFromFolder(TArray<FString>& string_array, const FString& path);
	UFUNCTION()
	TArray<FString> GetOtfFonts();
	UFUNCTION()
	TArray<FString> GetTtfFonts();


	void CreateDefaultFolder();
	void CreateTempFontsFolder();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	UPROPERTY()
	FString CurrentPropValue;

#endif

	FString GetConfigFilename();

	void LoadConfigFile();
	/*
	* Saves changed setting to config.*
	*/
	void SaveSettingToConfig();

	UFUNCTION()
	void MyOnResetDefaults();

	bool ResetToDefaults(FProperty* InProperty = nullptr);

	// UFUNCTION(CallInEditor, Category = Settings, meta = (DisplayPriority = 3))
	// void Reset();
	
	bool AlterFont(FNameProperty* Property, FName Value, FString Font);
	
	UPROPERTY(Config) 
	TMap<FName, FString> ToggleStates;

	
	void ConvertFont(FString InFontPath, FString OutputPath);
	void ConvertFontFolder(FProperty* InProperty, FString Path);
	bool HandleConversion(FFilePath InPath);
	void HandleConverterProp(FPropertyChangedEvent& PropertyChangedEvent);
	void ShowSuccess(FProperty* InProperty, float Duration);
	
	static void LaunchAndCaptureProcessOutput(const FString& FontForgePath, const FString& Args)
	{
	    UE_LOG(LogTemp, Log, TEXT("Launching process:"));
	    UE_LOG(LogTemp, Log, TEXT("Executable: %s"), *FontForgePath);
	    UE_LOG(LogTemp, Log, TEXT("Arguments: %s"), *Args);

	    // Create a pipe for capturing the output (stdout/stderr)
	    void* ReadPipe = nullptr;
	    void* WritePipe = nullptr;
	    if (!FPlatformProcess::CreatePipe(ReadPipe, WritePipe))
	    {
	        UE_LOG(LogTemp, Error, TEXT("Failed to create pipe for process output."));
	        return;
	    }

	    // Launch the process with our write pipe so its output is redirected
	    FProcHandle ProcHandle = FPlatformProcess::CreateProc(
	        *FontForgePath, 
	        *Args, 
	        false,      // Not detached
	        false,      // Not hidden
	        false,      // Not really hidden
	        nullptr,    // Process ID not needed
	        0,          // Default priority
	        nullptr,    // No working directory override
	        WritePipe   // Pass the write pipe for output capturing
	    );

	    if (!ProcHandle.IsValid())
	    {
	        UE_LOG(LogTemp, Error, TEXT("Failed to launch process."));
	        FPlatformProcess::ClosePipe(ReadPipe, WritePipe);
	        return;
	    }

	    // Continuously read the output until the process finishes
	    while (FPlatformProcess::IsProcRunning(ProcHandle))
	    {
	        // Read whatever is available from the pipe
	        FString NewOutput = FPlatformProcess::ReadPipe(ReadPipe);
	        if (!NewOutput.IsEmpty())
	        {
	            // Print output line by line (or however you prefer)
	            UE_LOG(LogTemp, Log, TEXT("%s"), *NewOutput);
	        }
	        // Sleep briefly to avoid a busy wait
	        FPlatformProcess::Sleep(0.1f);
	    }

	    // Read any remaining output after the process has terminated
	    FString RemainingOutput = FPlatformProcess::ReadPipe(ReadPipe);
	    if (!RemainingOutput.IsEmpty())
	    {
	        UE_LOG(LogTemp, Log, TEXT("%s"), *RemainingOutput);
	    }

	    // Close the pipe handles and process handle
	    FPlatformProcess::ClosePipe(ReadPipe, WritePipe);
	    FPlatformProcess::CloseProc(ProcHandle);

	    UE_LOG(LogTemp, Log, TEXT("Process has completed."));
	}
protected:
};
