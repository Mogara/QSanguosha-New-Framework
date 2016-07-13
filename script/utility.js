.pragma library

Array.prototype.contains = function(element) {
    for (var i = 0; i < this.length; i++) {
        if (this[i] === element)
            return true;
    }
    return false;
}
Array.prototype.removeOne = function(element){
    for (var i = 0; i < this.length; i++) {
        if (this[i] === element) {
            this.splice(i, 1);
        }
    }
}
Array.prototype.shuffle = function(){
    var result = [];
    while (this.length > 0){
        var ele = this[Math.floor(Math.random() * this.length)]
        result.push(ele);
        this.removeOne(ele);
    }
    return result;
}
