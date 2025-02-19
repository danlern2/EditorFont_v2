// Copyright 2024 Daniel Karasov Lerner. All Rights Reserved.

#include "UDevEditor.h"

#include "DebugHeader.h"
#include "Interfaces/IPluginManager.h"
#include "Logging/StructuredLog.h"
#include "HAL/PlatformTime.h"
#include "Misc/DateTime.h"
#include "Containers/Ticker.h"


UDevEditor::UDevEditor(const FObjectInitializer& ObjectInitializer)
    : PluginContentPath(*IPluginManager::Get().FindPlugin(TEXT("EditorFont"))->GetContentDir())
    , FontPath(*IPluginManager::Get().FindPlugin(TEXT("EditorFont"))->GetContentDir())
    , SlateFontPath(IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::EngineContentDir().Append("/Slate/Fonts")))
    , AltSlateFontPath(IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::EngineContentDir().Append("/Editor/Slate/Fonts")))
{
    FontPath = PluginContentPath + "/TempFonts";
    Defaults = PluginContentPath + "/Defaults";
    this->CreateDefaultFolder();
    this->CreateTempFontsFolder();
    CategoryName = "EditorFont";
    TSharedPtr<SWidget>UDevEditorWidget = this->GetCustomSettingsWidget();

    

}

FString UDevEditor::FilePicker(const FString& DialogueTitle)
{
    const void* NativeWinHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
    FString OutName;
    if (NativeWinHandle){
        IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
        if (DesktopPlatform) {
            DesktopPlatform->OpenDirectoryDialog(NativeWinHandle, DialogueTitle, FontPath, OutName);
        }
    }
    // FontPath = OutName;
    return OutName;
}

bool UDevEditor::GetFontsFromFolder(TArray<FString>& string_array, const FString& path)
{
    // this->LoadConfigFile();
    auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FJsonSerializableArray file_array;
    if (PlatformFile.DirectoryExists(*path)) {
        TArray<FString> Extensions = { TEXT(".otf"), TEXT(".ttf") };
        for (const FString& Extension : Extensions)
        {
            TArray<FString> TempFiles;
            PlatformFile.FindFiles(TempFiles, *path, *Extension);
            file_array.Append(TempFiles);
        }
    }
    else { string_array.Add("Path is invalid. Please double check."); return false; }
    if(file_array.Num() < 1){ string_array.Add("No valid fonts for this property in folder."); return false;}

    for (auto file : file_array) {
        FString PathPart = FString("");
        FString FileNamePart = FString("");
        FString ExtensionPart = FString("");

        FPaths::Split(*file, PathPart, FileNamePart, ExtensionPart);
        FName real_file = FName(FileNamePart + "." + ExtensionPart);
        string_array.Add(real_file.ToString());
    }
    return true;
}

TArray<FString> UDevEditor::GetOtfFonts()
{
    TArray<FString> string_array = TArray<FString>();
    TArray<FString> NewArray = TArray<FString>();
    if (!GetFontsFromFolder(string_array, FontPath)){
        return string_array;
    }
    for (auto font:string_array){
       if ( FPaths::GetExtension(*font, true) == TEXT(".otf")){
           NewArray.Add(font);
       }
    }
    return NewArray;
}

TArray<FString> UDevEditor::GetTtfFonts()
{
    TArray<FString> string_array = TArray<FString>();
    TArray<FString> NewArray = TArray<FString>();
    if (!GetFontsFromFolder(string_array, FontPath)){
        return string_array;
    }
    for (auto font:string_array){
        if ( FPaths::GetExtension(*font, true) == TEXT(".ttf")) {
            NewArray.Add(font);
        }
    }
    return NewArray;
}

void UDevEditor::CreateDefaultFolder()
{
    auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*Defaults)) {
        PlatformFile.CreateDirectory(*Defaults);
        PlatformFile.CopyDirectoryTree(*Defaults, *SlateFontPath, false);
        // Specify the path to the additional directory to copy files from.
        if (PlatformFile.DirectoryExists(*Defaults))
        {
             /*Create a custom directory visitor to copy files.*/
            class FFileCopyVisitor : public IPlatformFile::FDirectoryVisitor
            {
            public:
                FFileCopyVisitor(const FString& InDestinationFolder, IPlatformFile& InPlatform)
                    : DestinationFolder(InDestinationFolder)
                    , Platform(InPlatform)
                {
                }
                virtual bool Visit(const TCHAR* Filename, bool bIsDirectory) override
                {
                    // We want to copy only files (not subdirectories).
                    if (!bIsDirectory)
                    {
                        FString SourceFile(Filename);
                        FString FileName = FPaths::GetCleanFilename(SourceFile);
                        FString DestinationFile = FPaths::Combine(DestinationFolder, FileName);
                        // Copy the file from the source to destination.
                        if (!Platform.CopyFile(*DestinationFile, *SourceFile))
                        {
                            UE_LOG(LogTemp, Error, TEXT("Failed to copy file: %s"), *SourceFile);
                        }
                        else
                        {
                            UE_LOG(LogTemp, Log, TEXT("Copied file: %s"), *SourceFile);
                        }
                    }
                    return true; // continue iteration
                }
            private:
                FString DestinationFolder;
                IPlatformFile& Platform;
            };
            /*End of class definition scope*/
            FFileCopyVisitor Visitor(Defaults, PlatformFile);
            PlatformFile.IterateDirectory(*AltSlateFontPath, Visitor);
        }
    }
}

void UDevEditor::CreateTempFontsFolder()
{
    FPaths::NormalizeDirectoryName(FontPath);
    //NewFontFolderPath = FontPath + "/TempFonts";
    auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FText OutReason = FText::GetEmpty();
    if (!ToggleSettingsEditable || FontPath == FString("")) { return; }
    if (!PlatformFile.DirectoryExists(*FontPath))
    {
        if (!FPaths::ValidatePath(FontPath, &OutReason)) { UE_LOG(LogTemp, Error, TEXT("Critical failure! Reason: %s"), *OutReason.ToString()); return; }
        if (PlatformFile.CreateDirectory(*FontPath))
        {
            UE_LOG(LogTemp, Warning, TEXT("Folder created at %s"), *FontPath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Folder failed to be created at %s"), *FontPath);
        }
    }
    else{UE_LOG(LogTemp, Warning, TEXT("Folder found at %s"), *FontPath)}
}


void UDevEditor::PreEditChange(FProperty* PropertyAboutToChange)
{
    if (PropertyAboutToChange == nullptr) { return; }
    FName filename = FName("FileName");
    CurrentPropValue = PropertyAboutToChange->GetMetaDataText(filename).ToString();
    Super::PreEditChange(PropertyAboutToChange);
}

void UDevEditor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property == NULL) { return; }
    auto propertyName = PropertyChangedEvent.GetPropertyName();
    if (propertyName == GET_MEMBER_NAME_CHECKED(UDevEditor, FontChanger) || propertyName == TEXT("FilePath"))
    {
        HandleConverterProp(PropertyChangedEvent);
        return;
    }
    auto propertyNameString = propertyName.ToString();
    auto ptr = this->GetClass()->FindPropertyByName(propertyName)->ContainerPtrToValuePtr<FIntProperty>(this);
    if (propertyNameString == "ToggleSettingsEditable") { SaveSettingToConfig(); return; }
    FNameProperty* strProperty = CastField<FNameProperty>(PropertyChangedEvent.Property);
    auto propertyValue = strProperty->GetPropertyValue(ptr);
    FString Font = "";

    FName filename = FName("FileName");
    FString FontFileName = PropertyChangedEvent.Property->GetMetaDataText(filename).ToString(); // Access the FileName metadata

    // test this
    if (PropertyChangedEvent.Property->HasMetaData(filename))
    {
        UE_LOG(LogTemp, Warning, TEXT("Font modified! %s = %s"), *propertyNameString,
               *propertyValue.ToString());
        AlterFont(strProperty, propertyValue, FontFileName);
    }
    else if (propertyNameString.Equals("FontPath"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Font Path modified!"));
        this->CreateTempFontsFolder();
        FFilePath NewPath;
        
    }
    Super::PostEditChangeProperty(PropertyChangedEvent); 
    check(GEngine);
    this->SaveSettingToConfig();

}

FString UDevEditor::GetConfigFilename()
{
    //return FPaths::Combine(*FPaths::EnginePluginsDir(), TEXT("Marketplace"), TEXT("Config"), TEXT("EditorFontConfig.ini"));
    
    return FConfigCacheIni::NormalizeConfigIniPath(FPaths::Combine(FPaths::GetProjectFilePath(), TEXT("Intermediate"), TEXT("Config"), TEXT("CoalescedSourceConfigs"), TEXT("Setting.ini")));
}

void UDevEditor::LoadConfigFile()
{
    UDevEditor* Setting = GetMutableDefault<UDevEditor>();
    check(Setting);
    Setting->LoadConfig(Setting->GetClass(), *GetConfigFilename());

}


void UDevEditor::SaveSettingToConfig()
{
    UDevEditor* Setting = GetMutableDefault<UDevEditor>();
    check(Setting);
    Setting->SaveConfig(CPF_Config, *Setting->GetDefaultConfigFilename());
}

void UDevEditor::MyOnResetDefaults()
{
    if (!ToggleSettingsEditable) 
    {
        UE_LOG(LogTemp, Warning, TEXT("ToggleSettingsEditable is false"));
    }
    else if (this->ResetToDefaults())
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully reset to default fonts."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to reset to defaults."));
    }

}

bool UDevEditor::ResetToDefaults(FProperty* InProperty)
{
    auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    FString Font = "";
    int counter = 0;
    // Get the UClass of the object
    UClass* Class = this->GetClass();

    if (!Class)
    {
        UE_LOG(LogTemp, Warning, TEXT("Class is null"));
        return false;
    }

    /******************************************Singleton reset************************************/
    if (InProperty != nullptr)
    {
        
        FName filename = FName("FileName");
        // test this
        if (InProperty->HasMetaData(filename))
        {
            Font = InProperty->GetMetaDataText(filename).ToString(); // Access the FileName metadata
        }

        FName PropertyFName = InProperty->GetFName();
        auto ptr = this->GetClass()->FindPropertyByName(PropertyFName)->ContainerPtrToValuePtr<FNameProperty>(this);
        InProperty->ClearValue(ptr); 
        FString DestFontPath = InProperty->GetBoolMetaData("AlternateFont") ? FPaths::EngineContentDir() / TEXT("Editor/Slate/Fonts/") : FPaths::EngineContentDir() / TEXT("Slate/Fonts/");
        FString defaultFont = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(Defaults + "/" + Font));
        FString FontToChange = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(DestFontPath + "/" + Font));

        if (PlatformFile.DeleteFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange)))
        {
            if (!PlatformFile.CopyFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange), *defaultFont))
            {
                UE_LOG(LogTemp, Warning, TEXT("Property failed to reset. %s"), *PropertyFName.ToString());
                return false;
            }
        }
        else if (!PlatformFile.FileExists(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange)))
        {
            if (!PlatformFile.CopyFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange), *defaultFont))
            {
                UE_LOG(LogTemp, Warning, TEXT("Property failed to reset. %s"), *PropertyFName.ToString());
                return false;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("File failed to be deleted. %s"), *PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange));
            return false;
        }
        UE_LOG(LogTemp, Warning, TEXT("%s property reset"), *InProperty->GetName());
        FSlateApplication::Get().GetRenderer()->FlushFontCache(TEXT("Fonts have been updated."));
        return true;
    }
    //========================================================================================
    
    for (TFieldIterator<FProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
    {
        if (PropertyIt)
        {
            if (PropertyIt->GetFullName().Equals("ToggleStates"))
            {
                continue;
            }
            if (ToggleStates.Contains(PropertyIt->GetFName()))
            {
                if (ToggleStates[PropertyIt->GetFName()] == "False")
                {
                    continue;
                }
            }
            
            FName PropertyFName = PropertyIt->GetFName();
            FString PropertyName = PropertyFName.ToString();
            FString PropertyType = PropertyIt->GetClass()->GetName();
            if (PropertyFName == GET_MEMBER_NAME_CHECKED(UDevEditor, FontChanger) || PropertyFName == TEXT("FilePath"))
            {
                continue;
            }
            auto ptr = this->GetClass()->FindPropertyByName(PropertyFName)->ContainerPtrToValuePtr<FNameProperty>(this);
            
            FName filename = FName("FileName");
            if (PropertyIt->HasMetaData(filename))
            {
                Font = PropertyIt->GetMetaDataText(filename).ToString(); // Access the FileName metadata
            }
            else if (PropertyName.Equals("FontPath"))
            {
                FontPath = *IPluginManager::Get().FindPlugin(TEXT("EditorFont"))->GetContentDir().Append("/TempFonts");
                CreateTempFontsFolder();
                continue;
            }
            else if (PropertyName.Equals("FontChanger"))
            {
                continue;
            }
            else { continue; }
            
            FString DestFontPath = PropertyIt->GetBoolMetaData("AlternateFont") ? FPaths::EngineContentDir() / TEXT("Editor/Slate/Fonts/") : FPaths::EngineContentDir() / TEXT("Slate/Fonts/");
            PropertyIt->ClearValue(ptr);
            FString defaultFont = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(Defaults + "/" + Font));
            FString FontToChange = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(DestFontPath + "/" + Font));

            if (PlatformFile.DeleteFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange)))            {
                if (!PlatformFile.CopyFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange), *defaultFont))                {
                    UE_LOG(LogTemp, Warning, TEXT("Property failed to reset. %s"), *PropertyFName.ToString());
                    counter++;
                }
            }
            else if (!PlatformFile.FileExists(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange)))            {
                if (!PlatformFile.CopyFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange), *defaultFont))                {
                    UE_LOG(LogTemp, Warning, TEXT("Property failed to reset. %s"), *PropertyFName.ToString());
                    counter++;
                }
            }
            else            {
                UE_LOG(LogTemp, Warning, TEXT("File failed to be deleted. %s"), *PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange));
                counter++;
            }
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Encountered %d errors."), counter) // why do i not need a ; here
    FSlateApplication::Get().GetRenderer()->FlushFontCache(TEXT("Fonts have been updated."));
    return true;
}

// void UDevEditor::Reset()
// {
//     UE_LOG(LogTemp, Warning, TEXT("Reset Test"))
// }

bool UDevEditor::AlterFont(FNameProperty* Property, FName Value, FString Font)
{
    auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString DestFontPath = Property->GetBoolMetaData("AlternateFont") ? FPaths::EngineContentDir() / TEXT("Editor/Slate/Fonts/") : FPaths::EngineContentDir() / TEXT("Slate/Fonts/");
    FString TempFont = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(FontPath + "/" + Value.ToString()));
    FString FontToChange = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(DestFontPath + "/" + Font));

    if (PlatformFile.DeleteFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange)))
    {
        if (!PlatformFile.CopyFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange), *TempFont))
        {
            UE_LOG(LogTemp, Warning, TEXT("File failed to copy. %s"), *TempFont);
            this->ResetToDefaults(Property);
            return false;
        }
        FSlateApplication::Get().GetRenderer()->FlushFontCache(TEXT("Fonts have been updated."));
        UE_LOG(LogTemp, Warning, TEXT("Font changed successfully!"));
        return true;
    }
    else if (!PlatformFile.FileExists(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange)))
    {
        if (!PlatformFile.CopyFile(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange), *TempFont))
        {
            UE_LOG(LogTemp, Warning, TEXT("File failed to copy. %s"), *TempFont);
            this->ResetToDefaults(Property);
            return false;
        }
        FSlateApplication::Get().GetRenderer()->FlushFontCache(TEXT("Fonts have been updated."));
        UE_LOG(LogTemp, Warning, TEXT("Font changed successfully!"));
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("File failed to be deleted. %s"), *PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*FontToChange));
        this->ResetToDefaults(Property);
        return false;
    }
}


UDevEditor::~UDevEditor()
{
}

TSharedPtr<SWidget> UDevEditor::GetCustomSettingsWidget() const
{
    return TSharedPtr<SWidget>();
}

void UDevEditor::ConvertFont(FString InFontPath, FString OutputPath)
{
    if (!FPaths::FileExists(*InFontPath))
    {
        InFontPath = FPaths::Combine(FPaths::ProjectDir(), InFontPath);
    }
     auto TempP = InFontPath;
    FString Directory, Filename, Extension;
    FPaths::Split(OutputPath, Directory, Filename, Extension);
    OutputPath = FPaths::ChangeExtension(TempP, Extension);
    
    UE_LOG(LogTemp, Warning, TEXT("Converting font to %s"), *OutputPath);
    // Get the absolute PluginContentPath.
    FString AbsolutePluginContentPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*PluginContentPath);
    // Build the Python directory from PluginContentPath.
    FString PythonDir = FPaths::Combine(AbsolutePluginContentPath, TEXT("Python"));

    // Determine the FontForge executable name in a cross-platform way.
    FString ExecutableName = TEXT("fontforge");
#if PLATFORM_WINDOWS
    ExecutableName += TEXT(".exe");
#endif

    FString FontForgePath = FPaths::Combine(PythonDir, TEXT("FontForge"), TEXT("bin"), ExecutableName);
    FString ScriptFile = FPaths::Combine(PythonDir, TEXT("FontForge"), TEXT("bin"), TEXT("convert_otf_ttf.py"));
    
    // auto ScriptFileExists = FPaths::FileExists(*ScriptFile);
    // auto FontForgePathExists = FPaths::FileExists(*FontForgePath);
    // auto InPathExists = FPaths::FileExists(*InFontPath);
    // auto OutputPathExists = FPaths::FileExists(*OutputPath);
    // UE_LOGFMT(LogTemp, Warning, "In: {0} Out: {1} FFPath: {2} Script: {3}", InPathExists, OutputPathExists, FontForgePathExists, ScriptFileExists);
    if (FPaths::FileExists(*OutputPath))
    {
        DebugHeader::ShowNotifyInfo(FString("File already exists in directory"), SNotificationItem::CS_Fail);

        UE_LOGFMT(LogTemp, Warning, "File already exists in directory.");
        return;
    }
    
    FString Args = FString::Printf(TEXT("-lang=py -script \"%s\" \"%s\" \"%s\""), *ScriptFile, *InFontPath, *OutputPath);
    UE_LOG(LogTemp, Warning, TEXT("FontForge args: %s %s"), *FontForgePath, *Args);
    LaunchAndCaptureProcessOutput(FontForgePath, Args);

    FString Message = "Successfully converted font: ";
    Message.Append(Filename);
    

    DebugHeader::ShowNotifyInfo(FString(Message), SNotificationItem::CS_Success);
}

void UDevEditor::ConvertFontFolder(FProperty* InProperty, FString Path)
{
    bool bConverted;
    int ErrorCount = 0;
    if (FPaths::DirectoryExists(Path))
    {
        TArray<FString> Files; 
        GetFontsFromFolder(Files, Path);
        for (auto File : Files)
        {
            File = FPaths::Combine(Path, File);
            if (FPaths::FileExists(File))
            {
                FFilePath NewPath;
                NewPath.FilePath = File;
                bConverted = HandleConversion(NewPath);
                if (!bConverted)
                {
                    ErrorCount++;
                }
            }
        }
        if (ErrorCount == 0){ShowSuccess(InProperty, 3.0f);}
        else{ShowSuccess(InProperty, 0.01f); UE_LOG(LogTemp, Error, TEXT("Some files may have failed to convert. Encountered %d errors."), ErrorCount);}
    }
}

bool UDevEditor::HandleConversion(FFilePath InPath)
{
    FString Directory, Filename, Extension;
    FPaths::Split(InPath.FilePath, Directory, Filename, Extension);
    if (!Extension.Equals(TEXT("ttf"), ESearchCase::IgnoreCase) && !Extension.Equals(TEXT("otf"), ESearchCase::IgnoreCase))
    {
        return false;
    }
    FString NewExtension;
    if (Extension.Equals(TEXT("ttf"), ESearchCase::IgnoreCase))
    {
        NewExtension = TEXT(".otf");
    }
    else
    {
        NewExtension = TEXT(".ttf");
    }
    FString NewFileName = Filename + NewExtension;
    FString NewOutputPath = FPaths::Combine(Directory, NewFileName);

    ConvertFont(InPath.FilePath, NewOutputPath);
    return true;
}

void UDevEditor::HandleConverterProp(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property == NULL) { return; }
    auto propertyName = PropertyChangedEvent.GetPropertyName();
    if (propertyName == GET_MEMBER_NAME_CHECKED(UDevEditor, FontChanger) || propertyName == TEXT("FilePath"))  {
        bool bConverted = HandleConversion(FontChanger);
        if (!bConverted)  {
            UE_LOG(LogTemp, Error, TEXT("File conversion failed."));
        }
        else
        {
            ShowSuccess(PropertyChangedEvent.Property, 3.0f);
        }
    }
}

void UDevEditor::ShowSuccess(FProperty* InProperty, float Duration)
{
    if (InProperty == nullptr){return;}
    
    // Capture the starting time.
    const double StartTime = FPlatformTime::Seconds();
    auto StructProperty = InProperty->GetFName();
    if (StructProperty == GET_MEMBER_NAME_CHECKED(UDevEditor, FontChanger) || StructProperty == TEXT("FilePath"))
    {
        FontChanger.FilePath = "Successful Conversion";
        FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda(
                [StartTime, Duration, this](float DeltaTime) -> bool
                {
                    // Check if the elapsed time has reached the desired delay.
                    if (FPlatformTime::Seconds() - StartTime >= Duration)
                    {
                        // UE_LOG(LogTemp, Log, TEXT("Delayed event executed after %f seconds."), Duration);
                        FontChanger.FilePath = "";
                        // Return false to remove the ticker from the ticker list.
                        return false;
                    }
                    // Return true to continue ticking.
                    return true;
                }
            )
        );
          
    }
}



