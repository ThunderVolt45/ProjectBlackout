// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/BORavagerData.h"
#include "Combat/Weapons/BOEnemyProjectile.h"


bool FBossProjectileSettings::IsValid() const
{
	return ProjectileClass 
	  && ProjectileSpawnParams.Effect 
	  && ProjectileSpawnParams.DamageMagnitude > 0.f;
}
