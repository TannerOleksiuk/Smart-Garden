function toggle_pump()
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", "http://smart-garden/pump", true ); // false for synchronous request
    xmlHttp.send( null );
}

function toggle_lights()
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", "http://smart-garden/lights", true ); // false for synchronous request
    xmlHttp.send( null );
}