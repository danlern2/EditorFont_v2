#pragma once
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"


namespace DebugHeader {
	enum class ENotificationType {
		Info,
		Success,
		Error,
	};

	static inline EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true) {
		if (bShowMsgAsWarning) {
			FText MsgTitle = FText::FromString("Warning");
			return FMessageDialog::Open(MsgType, FText::FromString(Message), MsgTitle);
		}
		else {
			return FMessageDialog::Open(MsgType, FText::FromString(Message));
		}
	}

	static inline TSharedPtr<SNotificationItem> ShowNotifyInfo(const FString& Message, SNotificationItem::ECompletionState State = SNotificationItem::CS_None, float FadeOutDuration = 7.0f) {
		FNotificationInfo NotificationInfo(FText::FromString(Message));
		NotificationInfo.bUseLargeFont = true;

		NotificationInfo.FadeOutDuration = FadeOutDuration;

		TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);

		if(NotificationItem.IsValid()) {
			NotificationItem->SetCompletionState(State);
		}

		return NotificationItem;
	}
}
