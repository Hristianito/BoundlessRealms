#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_DEBUG_SPHERE(Location, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 16, Color, true) 
#define DRAW_DEBUG_SPHERE_Duration(Location, Color, Duration) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 16, Color, false, Duration) 
#define DRAW_DEBUG_SPHERE_SingleFrame(Location, Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 16, Color, false, -1.f) 
#define DRAW_DEBUG_LINE(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Emerald, true, -1.f, 0, 1.f)
#define DRAW_DEBUG_LINE_SingleFrame(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Emerald, false, -1.f, 0, 1.f)
#define DRAW_DEBUG_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 25.f, FColor::Emerald, true)
#define DRAW_DEBUG_POINT_SingleFrame(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 25.f, FColor::Emerald, false, -1.f, 0)
#define DRAW_DEBUG_CYLINDER(StartLocation, EndLocation) if (GetWorld()) DrawDebugCylinder(GetWorld(), StartLocation, EndLocation, 25.f, 16, FColor::Emerald, true, -1.f, 0, 1.f)
#define DRAW_DEBUG_CYLINDER_SingleFrame(StartLocation, EndLocation) if (GetWorld()) DrawDebugCylinder(GetWorld(), StartLocation, EndLocation, 25.f, 16, FColor::Emerald, false, -1.f, 0, 1.f)