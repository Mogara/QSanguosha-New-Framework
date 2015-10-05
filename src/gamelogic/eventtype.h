/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of QSanguosha.

    This game engine is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3.0
    of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE file for more details.

    Mogara
*********************************************************************/

#ifndef EVENTTYPE_H
#define EVENTTYPE_H

enum EventType
{
    InvalidEvent,

    GameStart,
    GameFinish,

    TurnStart,
    PhaseStart,
    PhaseProceeding,
    PhaseEnd,
    PhaseChanging,
    PhaseSkipping,
    TurnBroken,
    StageChange,

    BeforeCardsMove,
    CardsMove,

    DrawNCards,
    AfterDrawNCards,
    CountMaxCardNum,

    PreCardUsed,
    CardUsed,
    TargetChoosing,
    TargetConfirming,
    TargetChosen,
    TargetConfirmed,
    CardEffect,
    CardEffected,
    CardEffectConfirmed,
    PostCardEffected,
    CardFinished,
    TrickCardCanceling,

    CardResponded,

    SlashEffect,
    SlashEffected,
    SlashProceed,
    SlashHit,
    SlashMissed,

    ConfirmDamage,    // confirm the damage's count and damage's nature
    Predamage,        // trigger the certain skill -- jueqing
    DamageForseen,    // the first event in a damage -- kuangfeng dawu
    DamageCaused,     // the moment for -- qianxi..
    DamageInflicted,  // the moment for -- tianxiang..
    PreDamageDone,    // before reducing Hp
    DamageDone,       // it's time to do the damage
    Damage,           // the moment for -- lieren..
    Damaged,          // the moment for -- yiji..
    DamageComplete,   // the moment for trigger iron chain

    PreHpRecover,
    HpRecover,

    MaxHpChanged,

    StartJudge,
    AskForRetrial,
    FinishRetrial,
    FinishJudge,

    EventTypeCount
};

#endif // EVENTTYPE_H
