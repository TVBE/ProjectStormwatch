#pragma once
#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"
#include "DetailCategoryBuilder.h"


class ReacousticStaticMeshDetailCustomization : public IDetailCustomization
{
	public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
