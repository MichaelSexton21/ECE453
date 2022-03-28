// JavaScript Document

var Brush = {
  Running: false,
  ForwardPrev: 1,
  GateDriverPrev: 0,
  DeviceModePrev: 1
};

$(function() {
  $("#direction").buttonset();   
  $("#deviceMode").buttonset();
  $("#gateDriver").buttonset();  
    
  $( "#slider" ).slider({
    range: "min",
    min: 0,
    max: 100,
    value: 75,
    step: 1,
    change: function(event, ui) {
        var actualSpeed         
        var cmdRes;
        
        actualSpeed = $("#slider").slider("option", "value");
        $("#speedLabel").text(actualSpeed + ' %');
        
        ChangeSpeed(actualSpeed);
    }
  });
});

/* 
 * Enable or disable control buttons.
 */
function SetButtons(Running)
{        
  if (Brush.Running) {
    document.getElementById("runBtn").disabled = true;
    document.getElementById("brakeBtn").disabled = false;
    document.getElementById("freewheelBtn").disabled = false;
  }
  else {
    document.getElementById("runBtn").disabled = false;
    document.getElementById("brakeBtn").disabled = true;
    document.getElementById("freewheelBtn").disabled = true;
  }
}
  
function RunMotor()
{
  var Forward;
  var Speed;
  var cmdRes;
  
  Brush.Running = true;
  SetButtons(Brush.Running);

  Forward = $("input[name='direction']:checked").val();  
  Speed = $("#slider").slider("option", "value"); 
  Args = Forward + "," + Speed; 
    
  /* alert("Run: " + Args); */
  if (!fmstr.SendCommand("RotateProportional(" + Args + ")", cmdRes)) {
    alert("Error in jscript code!");
  }
}

function BrakeMotor()
{
  var cmdRes;
  
  Brush.Running = false;
  SetButtons(Brush.Running);
  
  if (!fmstr.SendCommand("Brake()", cmdRes)) {
    alert("Error in jscript code!");
  }
}

function FreewheelMotor()
{
  var cmdRes;
  
  Brush.Running = false;
  SetButtons(Brush.Running);
  
  if (!fmstr.SendCommand("SetTriState()", cmdRes)) {
    alert("Error in jscript code!");
  }
}

function ChangeDirection()
{
  var Forward;
  var Speed;
  var Args;
  var cmdRes;

  if (Brush.Running == true) {
    Forward = $("input[name='direction']:checked").val();
    
    if (Forward != Brush.ForwardPrev) {
      Brush.ForwardPrev = Forward;                   
      Speed = $("#slider").slider("option", "value");
      Args = Forward + "," + Speed;
      
      /* alert("SetDirection: " + Args); */
      if (!fmstr.SendCommand("SetDirection(" + Args + ")", cmdRes)) {
        alert("Error in jscript code!");
      }
    }
  }
}

function ChangeSpeed(Speed)
{
  var cmdRes;
  var Speed;

  if (Brush.Running) {
    /* alert("SetSpeed: " + Speed); */
    if (!fmstr.SendCommand("SetSpeed(" + Speed + ")", cmdRes)) {
      alert("Error in jscript code!");
    }
  }
}

function ChangeDeviceMode()
{
  var cmdRes;
  var Mode;
  
  Mode = $("input[name='deviceMode']:checked").val();  
  
  if (Mode != Brush.DeviceModePrev) {
    Brush.DeviceModePrev = Mode;    
    /* alert("SetMode: " + Mode); */
    if (!fmstr.SendCommand("SetMode(" + Mode + ")", cmdRes)) {
      alert("Error in jscript code!");
    }
  }
}

function ChangeGateDriver()
{
  var cmdRes;
  var OutVal;
  
  OutVal = $("input[name='gateDriver']:checked").val();
  
  if (OutVal != Brush.GateDriverPrev) { 
    Brush.GateDriverPrev = OutVal;
    /* alert("SetGateDriver: " + OutVal); */
    if (!fmstr.SendCommand("SetGateDriver(" + OutVal + ")", cmdRes)) {
      alert("Error in jscript code!");
    }
  }
}

function PageInit()
{    
  var result
  /* Initialize GUI. */
  SetButtons(Brush.Running);
  /* Initialize speed label. */
  var actualSpeed = $("#slider").slider("option", "value");
  $("#speedLabel").text(actualSpeed + ' %');
}
