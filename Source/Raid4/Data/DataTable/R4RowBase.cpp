

#include "R4RowBase.h"

#if WITH_EDITOR
#include <Misc/MessageDialog.h>
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4RowBase)

/**
 * Can be overridden by subclasses; Called on every row when the owning data table is modified
 * Allows for custom fix-ups, parsing, etc for user changes
 * This will be called in addition to OnPostDataImport when importing
 *
 * @param InDataTable					The data table that owns this row
 * @param InRowName						The name of the row we're performing fix-up on
 */
void FR4RowBase::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);

#if WITH_EDITOR
	// 에디터에서 수정할 경우, Primary Key의 중복 또는 Invalid 검사를 진행 후 알려준다.
	if(!InDataTable)
		return;

	// Invalid PK (0) 인 경우
	if(PrimaryKey == DTConst::G_InvalidPK)
	{
		FText title = FText::FromString(FString::Printf(TEXT("%s Warning!"), *InDataTable->GetName()));
		FText text = FText::FromString(FString::Printf(TEXT("%s, PK : %d is invalid number."), *InRowName.ToString(), PrimaryKey));
		FMessageDialog::Open(EAppMsgType::Ok, text, title);

		return;
	}

	// 중복된 PK인 경우
	for(const auto& [rowName, rowData] : InDataTable->GetRowMap())
	{
		if(const FR4RowBase* data = reinterpret_cast<FR4RowBase*>(rowData))
		{
			if(rowName == InRowName)
				continue;
			
			if(PrimaryKey == data->PrimaryKey)
			{
				FText title = FText::FromString(FString::Printf(TEXT("%s Warning!"), *InDataTable->GetName()));
				FText text = FText::FromString(FString::Printf(TEXT("%s, PK : %d is duplicate number."), *InRowName.ToString(), PrimaryKey));
				FMessageDialog::Open(EAppMsgType::Ok, text, title);

				PrimaryKey = DTConst::G_InvalidPK;
				return;
			}
		}
	}
#endif
}
