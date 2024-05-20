// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/GridPanel.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "AvatarPawn.h"
#include "AvatarsTypes.h"
#include "UserInterface/CharacterThumbnailWidget.h"
#include "UserInterface/DebugInterfaceWidget.h"
#include "UserInterface/Localisation/LanguageSwitcherWidget.h"
#include "UserInterface/OptionsMenu.h"

#include "RootWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterThumbnailClickDelegate, const FAvatarId&);

class AAvatarsPlayerController;
class UDataTable;
class ULoadingScreenWidget;

UCLASS()
class AVATARS_API URootWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;

  // This is called every time that the widget is compiled,
  // or a property is changed.
  virtual void SynchronizeProperties() override;

  UFUNCTION(BlueprintCallable)
  void SetupDebugHelpers();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bDebug;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bLogKeyDown;

  bool GetController();
  AAvatarsPlayerController* PlayerController;

  /* Add to this list widgets should be visible in debug mode and hidden (collapsed) if not. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<UWidget*> DebugHelpersWidgets;

  /* Add to this list widgets that should be hidden when the app starts by default. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<UWidget*> WidgetsHiddenByDefault;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UCanvasPanel* CanvasPanel;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UDebugInterfaceWidget* DebugInterface;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* UserMessage;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UVerticalBox* Suggestions;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* SuggestionsText;

  UFUNCTION(BlueprintCallable)
  void ShowSuggestionsText();

  UFUNCTION(BlueprintCallable)
  void HideSuggestionsText();

  UFUNCTION(BlueprintCallable)
  void SetSuggestionsText(const TArray<FString>& NewSuggestions);

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* StartConversationButton;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  ULoadingScreenWidget* LoadingScreen2;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UOverlay* CharacterSelection;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* BackToCharacterSelectionButton;

  UFUNCTION()
  void OnBackToCharacterSelection();

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  ULanguageSwitcherWidget* LanguageSwitcher;

  UFUNCTION()
  void OnLanguageSwitch(EAvatarLanguage Language);

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* StatusMessge;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UCharacterThumbnailWidget* JanZumbachThumbnail2;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UCharacterThumbnailWidget* JakKowalewskiThumbnail2;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UCharacterThumbnailWidget* WojtekTheBearThumbnail;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UOverlay* UserTextInputBox;

  UFUNCTION(BlueprintCallable)
  void HideUserTextBox();

  UFUNCTION(BlueprintCallable)
  void ShowUserTextBox();

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UMultiLineEditableTextBox* UserTextBox;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* SendTextButton;

  UFUNCTION(BlueprintCallable)
  void OnSendTextButtonClick();

  /* This button should be displayed only when in editor or development builds. */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* DebugOpenOptionsButton;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UOptionsMenu* OptionsMenu;

  /* Create this table in editor and assign to blueprint widget created of this RootWidget class. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
  UDataTable* TranslationsTable;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<UCharacterThumbnailWidget> ThumbsClass;
  TArray<UCharacterThumbnailWidget*> AvatarsThumbnails;

  UFUNCTION(BlueprintCallable)
  void DisplayUserMessage(FString Message);

  UFUNCTION(BlueprintCallable)
  void StartNewConversation();

  UFUNCTION(BlueprintCallable)
  void ShowLoadingScreen();

  UFUNCTION(BlueprintCallable)
  void HideLoadingScreen();

  UFUNCTION(BlueprintCallable)
  void ShowRecordingInProgressMessage();

  UFUNCTION(BlueprintCallable)
  void ShowPressToTalkMessage();

  UFUNCTION(BlueprintCallable)
  void HideUserMessage(const float Delay);

  UFUNCTION(BlueprintCallable)
  void SelectThumbnailById(const FAvatarId& Id);

  UFUNCTION(BlueprintCallable)
  void UpdateThumbnailsDisplay();

  UFUNCTION(BlueprintCallable)
  void OnCharacterThumbnailClick(const FAvatarId& Id);

  UFUNCTION(BlueprintCallable)
  void CreateAvatarsThumbnails(TArray<FAvatarData> AvatarsData);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  EAvatarLanguage GetAvatarLanguage();

  UFUNCTION(BlueprintCallable)
  void SetLanguage(EAvatarLanguage InLanguage);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  FText GetTranslation(FName Key);

  FOnCharacterThumbnailClickDelegate OnCharacterThumbnailClickEvent;

  virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
