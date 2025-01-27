#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBar;

UCLASS()
class BOUNDLESSREALMS_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	void SetHealthPercent(float Percent);

private:

	UPROPERTY()
	UHealthBar* HealthBarWidget;
};
