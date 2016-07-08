import QtQuick 2.5
import Sanguosha.CardArea 1.0
import Sanguosha.Player 1.0
import Sanguosha.GameLogic 1.0


QtObject {
    id: gameLogicMain

    property GameLogic gameLogicCpp;

    property var eventHandlers;
    property var players;
    property ServerPlayer currentPlayer;
    property var extraTurns;
    property GameRule gameRule;
    property var packages;
    property var cards;
    property bool skipGameRule;
    property int round;
    property int reshufflingCount;

    property CardArea drawPile;
    property CardArea discardPile;
    property CardArea tablePile;
    property CardArea wuguPile;

    property var cardPosition;

    function loadMode(modeName) // use C++?
    {

    }

    function trigger(triggerEvent, player, data)
    {

    }

    function init()
    {
        currentPlayer = null;
        gameRule = null;
        skipGameRule = false;
        round = 0;
        reshufflingCount = 0;
        players = new Array;
        extraTurns = new Array;
        packages = new Array;

        drawPile = new CardArea(CardArea.DrawPile);
        discardPile = new CardArea(CardArea.DiscardPile);
        tablePile = new CardArea(CardArea.Table);
        tablePile.setIsVirtualCardArea(true);
        wuguPile = new CardArea(CardArea.Wugu);

        // load game mode
        // loadMode(gameLogicCpp.mode);

        var playerList = gameLogicCpp.playersList();
        for (var i = 0; i < playerList.length; ++i)
            players[i] = playerList[i];

        for (i = 0; i < players.length - 1; ++i) {
            var j = Math.floor(Math.random() * (players.length - i + 1)) + i;
            if (i != j) {
                var t = players[i];
                players[i] = players[j];
                players[j] = t;
            }
        }

        for (i = 0; i < players.length - 1; ++i) {
            players[i].setSeat(i + 1);
            players[i].setNext(players[i + 1]);
        }
        players[players.length - 1].setSeat(players.length);
        players[players.length - 1].setNext(players[0]);
        currentPlayer = players[0];

        gameLogicCpp.broadcastSeats();

        // load general list
        // load cards map
        // broadcast card

        cards = {
            count: 0,
            ids: new Array,
            cards: new Array
        };

        cardPosition = {};

        for (i = 0; i < cards.count; ++i) {
            drawPile.add(cards.cards[i]);
            cardPosition[cards.cards[i]] = drawPile;
        }

        gameRule.prepareToStart(this);

        eventHandlers = {}
    }

    function run()
    {
        for (;;) {
            try {
                var current = currentPlayer;
                for (;;) {
                    if (current.seat === 1)
                        round += 1;
                    if (current.dead) {
                        current = current.next();
                        continue;
                    }

                    currentPlayer = current;
                    trigger(GameLogic.TurnStart, current, null);
                    current = current.next;

                    while (extraTurns.length > 0) {
                        var extra = extraTurns.shift();
                        currentPlayer = extra;
                        trigger(GameLogic.TurnStart, current, null);
                    }
                }
            } catch (triggerEvent) {
                if (triggerEvent === GameLogic.GameFinish)
                    return;
                if (triggerEvent === GameLogic.TurnBroken) {
                    var turnBrokenCurrent = currentPlayer;
                    trigger(GameLogic.TurnBroken, turnBrokenCurrent, null);
                    var next = current.nextAlive(1, false);
                    if (turnBrokenCurrent.phase !== Player.Inactive) {
                        // gameRule.effect(this, GameLogic.PhaseEnd, new Event, null, turnBrokenCurrent);
                        // @todo:
                        turnBrokenCurrent.phase = Player.Inactive;
                        turnBrokenCurrent.broadcastProperty("phase");
                    }
                    currentPlayer = next;
                }
            }
        }
    }
    function setGameRule(rule){
        if (gameRule) {
                for (var e in gameRule.events) {
                    if (eventHandlers[e].contains(gameRule))
                        eventHandlers[e].remove(gameRule);
                }
            }

            gameRule = rule;
            if (rule) {
                for (var _e in rule.events)
                    eventHandlers[_e].push(rule);
            }
    }
    function setPackages(pkgs){
        if (pkgs instanceof Array)
            if (pkgs[1] instanceof String){
                var packagPtrs = [];
                for (var name in packages){
                    var pac = Sanguosha.package(name);
                    if (pac)
                        packagPtrs.push(pac);
                }
                packages = packagePtr;
            } else {
                packages = pkgs;
            }
    }
    function addEventHandler(evthdr){
        for (var e in evthdr.events){
            if (!eventHandlers[e].contains(evthdr)){
                eventHandlers[e].push(evthdr);
            }
        }
    }
    function removeEventHandler(evthdr){
        for(var e in evthdr.events){
            eventHandlers[e].remove(evthdr);
        }
    }

    function getEventHandlersAndSort(event,detailsList,triggered,data,player){
         // used to get all the event handlers which can be triggered now, and sort them.
        // everytime this function is called, it will get all the event handlers and judge the triggerable one by one
        var skillList = eventHandlers[event];
        var details;// We create a new list everytime this function is called
        for (var eh in skillList){
            // judge every Event Handler
            var triggerable = eh.triggerable(this,event,data,player);

        }

        //    QList<QSharedPointer<Event> > details;
        //    foreach (const EventHandler *eh, skillList) {
        //        // judge every Event Handler
        //        EventList triggerable = eh->triggerable(this, event, data, player);

        //        QMutableListIterator<Event> it_triggerable(triggerable);
        //        while (it_triggerable.hasNext()) {
        //            const Event &t = it_triggerable.next();
        //            if (!t.isValid())
        //                it_triggerable.remove();  // remove the invalid item from the list
        //        }

        //        if (triggerable.isEmpty()) // i.e. there is no valid item returned from the skill's triggerable
        //            continue;

        //        EventPtrList r; // we create a list for every skill
        //        foreach (const Event &t, triggerable) {
        //            // we copy construct a new Event in the heap area (because the return value from triggerable is in the stack). use a shared pointer to point to it, and add it to the new list.
        //            // because the shared pointer will destroy the item it point to when all the instances of the pointer is destroyed, so there is no memory leak.
        //            EventPtr ptr(new Event(t));
        //            r << ptr;
        //        }
        //        if (r.length() == 1) {
        //            // if the skill has only one instance of the invokedetail, we copy the tag to the old instance(overwrite the old ones), and use the old instance, delete the new one
        //            foreach (const EventPtr &detail, (detailsList + triggered).toSet()) {
        //                if (detail->sameSkill(*r.first())) {
        //                    foreach (const QString &key, r.first()->tag.keys())
        //                        detail->tag[key] = r.first()->tag.value(key);
        //                    r.clear();
        //                    r << detail;
        //                    break;
        //                }
        //            }
        //        } else {
        //            bool isPreferredTargetSkill = false;
        //            EventPtrList s;
        //            // judge whether this skill in this event is a preferred-target skill, make a invoke list as s
        //            foreach (const EventPtr &detail, (detailsList + triggered).toSet()) {
        //                if (detail->eh == r.first()->eh) {
        //                    s << detail;
        //                    if (detail->preferredTarget != nullptr)
        //                        isPreferredTargetSkill = true;
        //                }
        //            }
        //            if (!isPreferredTargetSkill) {
        //                std::sort(s.begin(), s.end(), [](const EventPtr &a1, const EventPtr &a2) { return a1->triggered && !a2->triggered; });
        //                // because these are of one single skill, so we can pick the invoke list using a trick like this
        //                s.append(r);
        //                r = s.mid(0, r.length());
        //            } else {
        //                // do a stable sort to r and s since we should judge the trigger order
        //                static std::function<bool(const EventPtr &, const EventPtr &)> preferredTargetLess =
        //                    [](const EventPtr &a1, const EventPtr &a2) {
        //                        return a1->preferredTargetLess(*a2);
        //                    };

        //                std::stable_sort(r.begin(), r.end(), preferredTargetLess);
        //                std::stable_sort(s.begin(), s.end(), preferredTargetLess);

        //                {
        //                    QListIterator<EventPtr> r_it(r);
        //                    QMutableListIterator<EventPtr> s_it(s);
        //                    while (r_it.hasNext() && s_it.hasNext()) {
        //                        QSharedPointer<Event> r_now = r_it.next();
        //                        QSharedPointer<Event> s_now = s_it.next();

        //                        if (r_now->preferredTarget == s_now->preferredTarget)
        //                            continue;

        //                        // e.g. let r =  a b c d e f   h     k
        //                        //      let s =  a b   d e f g h i j
        //                        // it pos:      *

        //                        // the position of Qt's Java style iterator is between 2 items, we can use next() to get the next item and use previous() to get the previous item, and move the iterator according to the direction.

        //                        if (ServerPlayer::sortByActionOrder(r_now->preferredTarget, s_now->preferredTarget)) {
        //                            // 1.the case that ServerPlayer::compareByActionOrder(r_now, s_now) == true, i.e. seat(r_now) < seat(s_now)
        //                            // because the r is triggerable list, s is the invoke list, so we should move s_it to the front of the just passed item add the r_now into s

        //                            // the list is now like this: r = a b c d e f   h
        //                            //                            s = a b   d e f g h i j
        //                            // it pos:                             r s

        //                            s_it.previous();
        //                            s_it.insert(r_now);

        //                            // so the list becomes like:  r = a b c d e f   h
        //                            //                            s = a b c d e f g h i j
        //                            // it pos:                             *
        //                        } else {
        //                            // 2. the case that ServerPlayer::compareByActionOrder(r_now, s_now) == true, i.e. seat(r_now) > seat(s_now)
        //                            // because the r is triggerable list, s is the invoke list, so we should remove the s_now and move r_it to the position just before the deleted item

        //                            // the list is now like this: r = a b c d e f   h
        //                            //                            s = a b c d e f g h i j
        //                            // it pos:                                     s r

        //                            s_it.remove();
        //                            r_it.previous();
        //                            // so the list becomes like:  r = a b c d e f   h
        //                            //                            s = a b c d e f   h i j
        //                            // it pos:                                   r s
        //                        }
        //                    }

        //                    // the whole loop will be over when one of r_it or s_it has no next item, but there are situations that another one has more items. Let's deal with this situation.
        //                    // let's take some other examples.

        //                    // e.g. let r = a b c d e
        //                    //      let s = a b c
        //                    // it pos:           *

        //                    // now s_it has no next item, but r_it has some next items.
        //                    // since r is the trigger list, we should add the more items to s.

        //                    while (r_it.hasNext())
        //                        s_it.insert(r_it.next());

        //                    // another example.

        //                    // e.g. let r = a b c
        //                    //          s = a b c d e
        //                    // it pos:           *

        //                    // now s_it has more items.
        //                    // since r is the triggerable list, we should remove the more items from s.
        //                    while (s_it.hasNext())
        //                        s_it.remove();
        //                }

        //                // let the r become the invoke list.
        //                r = s;

        //                // we should mark the ones who passed the trigger order as triggered.
        //                QListIterator<EventPtr> r_it(r);
        //                r_it.toBack();
        //                bool over_trigger = true;
        //                while (r_it.hasPrevious()) {
        //                    const EventPtr &p = r_it.previous();
        //                    if (p->triggered)
        //                        over_trigger = true;
        //                    else if (over_trigger)
        //                        p->triggered = true;
        //                }

        //            }

        //        }

        //        details << r;
        //    }

        //    // do a stable sort to details use the operator < of Event in which judge the priority, the seat of invoker, and whether it is a skill of an equip.
        //    std::stable_sort(details.begin(), details.end(), [](const EventPtr &a1, const EventPtr &a2) { return *a1 < *a2; });

        //    // mark the skills which missed the trigger timing as it has triggered
        //    EventPtr over_trigger;
        //    QListIterator<EventPtr> it(details);
        //    it.toBack();
        //    while (it.hasPrevious()) {
        //        // search the last skill which triggered times isn't 0 from back to front. if found, save it to over_trigger.
        //        // if over_trigger is valid, then mark the skills which missed the trigger timing as it has triggered.
        //        const EventPtr &detail = it.previous();
        //        if (over_trigger.isNull() || !over_trigger->isValid()) {
        //            if (detail->triggered)
        //                over_trigger = detail;
        //        } else if (*detail < *over_trigger)
        //            detail->triggered = true;
        //    }

        //    detailsList = details;
    }
}
