.pragma library

var kingdomColor = {
    wei : "#547998",
    shu : "#D0796C",
     wu : "#4DB873",
    qun : "#8A807A",
    god : "#96943D",
unknown : "#96943D"
};

function convertNumber(number)
{
    if (number === 1)
        return "A";
    if (number >= 2 && number <= 10)
        return number;
    if (number >= 11 && number <= 13) {
        var strs = ["J", "Q", "K"];
        return strs[number - 11];
    }
    return "";
}
