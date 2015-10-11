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
    AfterCardsMove,

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
    PostCardEffected,
    CardFinished,
    TrickCardCanceling,

    CardResponded,

    SlashEffect,
    SlashEffected,
    SlashProceed,
    SlashHit,
    SlashMissed,

    ConfirmDamage, // confirm the source, weight and nature of a damage
    BeforeDamage,  // trigger certain skill -- jueqing
    DamageStart,
    Damaging,
    Damaged,
    AfterDamaging,
    AfterDamaged,
    DamageComplete,

    BeforeRecover,
    AfterRecover,

    HpLost,
    AfterHpLost,

    BeforeHpReduced,
    AfterHpReduced,

    MaxHpChanged,

    StartJudge,
    AskForRetrial,
    FinishRetrial,
    FinishJudge,

    EnterDying,
    QuitDying,
    AskForPeach,
    AskForPeachDone,
    Died,
    BuryVictim,
    GameOverJudge,

    EventTypeCount
};

#endif // EVENTTYPE_H
