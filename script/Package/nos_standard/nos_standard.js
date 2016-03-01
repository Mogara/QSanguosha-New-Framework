
var nos_jianxiong = {
	triggerable: function(skill, logic, event, damage, player) {
		if (damage.to != null && damage.to.alive && damage.to.hasSkill(skill)) {
			var card = damage.card;
			if (card != null && logic.table().contains(card))
				return Sanguosha.newEvent(logic, skill, damage.to, damage.to);
		}
		
		return null;
	} ,
	cost: true ,
	effect: function(skill, logic, event, eventPtr, damage, player) {
		var moves = Sanguosha.newDataValue("CardsMoveValue");
		var move = Sanguosha.newDataValue("CardMove");
		move.toArea = damage.to.handCardArea();
		move.card = damage.card;
		move.isOpen = true;
		moves.addSubMove(move);
		logic.moveCards(moves);
		
		return false;
	}
};
// we should make the package, and the generals in the package, and the skills in a global variable.
// if we don't do like this, the skill instance will probably be cleared by the GC

var nosStandardPackage = null;
var nosStandardGenerals = new Array;
var nosStandardSkills = new Array;

function newPackagenos_standard()
{
    var nosStandardPackage = Sanguosha.newPackage("nos_standard");

	var caocao = Sanguosha.newGeneral("nos_caocao", "wei", 4, General.Male);
	var jianxiong = Sanguosha.newSkill("TriggerSkill", "nos_jianxiong");
	jianxiong.addEvent(EventHandler.AfterDamaged);
	caocao.addSkill(jianxiong);
	nosStandardGenerals[0] = caocao;
	nosStandardSkills[0] = jianxiong;
	
	nosStandardPackage.addGeneral(caocao);
	
    return nosStandardPackage;
}
