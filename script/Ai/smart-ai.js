function init()
{
	aiEngineFunctions.notifyToRobot(6, "init")
	return true;
}

function request(command, data)
{
	aiEngineFunctions.notifyToRobot(6, "requested")
}

function reply(command, data)
{
	aiEngineFunctions.notifyToRobot(6, "replyed")
}

function notify(command, data)
{
	aiEngineFunctions.notifyToRobot(6, "notified")
}