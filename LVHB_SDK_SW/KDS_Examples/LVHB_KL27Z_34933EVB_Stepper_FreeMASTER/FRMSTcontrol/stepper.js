// JavaScript Document

$(function() {
  $("#controlMode").buttonset();   
  $( "#fullSlider" ).slider({
    range: "min",
    min: 1,
    max: 500,
    value: 50,
    step: 1,
    change: function(event, ui) {
        var actualSpeed         
        var cmdRes;
        
        actualSpeed = $("#fullSlider").slider("option", "value");
        $("#fullSpeedLabel").text(actualSpeed + ' Full-steps/s');
    }
  });
  $( "#microSlider" ).slider({
    range: "min",
    min: 1,
    max: 500,
    value: 50,
    step: 1,
    change: function(event, ui) {
        var actualSpeed         
        var cmdRes;
        
        actualSpeed = $("#microSlider").slider("option", "value");
        $("#microSpeedLabel").text(actualSpeed + ' Micro-steps/s');
    }
  });
  
  /* Handle change of stepper mode (full/micro) */
  $('input:radio[name=controlMode]').change(function () {
    SetControlBox();
  });
  
  /* Continual full-step mode. */
  $('#fullContinual').click(function(){
    if($(this).is(':checked')) {
      /* Hide settings for number of steps. */
      document.getElementById("fullSteps").disabled = true;
    } 
    else {
      document.getElementById("fullSteps").disabled = false;
    }
  });
  
  /* Continual micro-step mode. */
  $('#microContinual').click(function(){
    if($(this).is(':checked')) {
      /* Hide settings for number of steps. */
      document.getElementById("microSteps").disabled = true;
    } 
    else {
      document.getElementById("microSteps").disabled = false;
    }
  });
});

function Led_On(led)
{
   document.getElementById(led).src = "images/greenLed.png";
}

function Led_Off(led)
{
   document.getElementById(led).src = "images/noneLed.png";
}

function SetControlBox()
{
  if ($("input[name='controlMode']:checked").val() == 'FULL_STEP') {
    /* Hide micro-step configuration and show full-step. */
    document.getElementById('fullSettings').style.display = 'block';
    document.getElementById('microSettings').style.display = 'none';
  }
  else if ($("input[name='controlMode']:checked").val() == 'MICRO_STEP') {
    /* Hide full-step configuration and show micro-step. */
    document.getElementById('fullSettings').style.display = 'none';
    document.getElementById('microSettings').style.display = 'block';
  }
}

/* Enable or disable all control buttons.
 * 
 */
function DisableButtons(disabled)
{
  document.getElementById("runBtn").disabled = disabled;
  document.getElementById("resetBtn").disabled = disabled;
  document.getElementById("alignBtn").disabled = disabled;
}

var LastStepSize = 32;
  
function RunMotor()
{
  var Forward;
  var Steps;
  var Speed;
  var StepSize;
  var cmdRes;
  
  if ($("input[name='controlMode']:checked").val() == 'FULL_STEP') {
    /* Use full-step mode. */
    Forward = $("#fullDirection").val();  
    Speed = $("#fullSlider").slider("option", "value"); 
    
    if ($('#fullContinual').is(':checked') == true) {
      /* Continual mode. */
      Args = Forward + "," + Speed;
      if (!fmstr.SendCommand("MoveContinual(" + Args + ")", cmdRes)) {
        alert("Error in jscript code!");
      }
    } 
    else { /* Not continual mode (steps no specified) */
      Steps = $("#fullSteps").val();
      Args = Forward + "," + Steps + "," + Speed; 
      
      //alert("FullStep: " + Args);
      if (!fmstr.SendCommand("MoveSteps(" + Args + ")", cmdRes)) {
        alert("Error in jscript code!");
      }
    }
  }
  else {
    /* Use micro-step mode. */
    Forward = $("#microDirection").val();
    Speed = $("#microSlider").slider("option", "value");
    StepSize = $("#microSize").val();
    
    if ($('#microContinual').is(':checked') == true) {
      /* Continual mode. */
      Args = Forward + "," + Speed + "," + StepSize;
      if (!fmstr.SendCommand("MoveMicroContinual(" + Args + ")", cmdRes)) {
        alert("Error in jscript code!");
      }
    } 
    else {
      Steps = $("#microSteps").val();
      Args = Forward + "," + Steps + "," + Speed + "," + StepSize;
      
      //alert("MicroStep: " + Args);
      if (!fmstr.SendCommand("MoveMicroSteps(" + Args + ")", cmdRes)) {
        alert("Error in jscript code!");
      }
    }
    
    LastStepSize = StepSize;
  }
}

function StopMotor()
{
  var cmdRes;
  if (!fmstr.SendCommand("StopContinual()", cmdRes)) {
    alert("Error in jscript code!");
  }
}

function DisableMotor()
{
  var cmdRes;
  if (!fmstr.SendCommand("DisableMotor()", cmdRes)) {
    alert("Error in jscript code!");
  }
}

function ResetPosition()
{
  var cmdRes;
  if (!fmstr.SendCommand("ResetPosition()", cmdRes)) {
    alert("Error in jscript code!");
  }
}

function AlignRotor()
{
  var cmdRes;
  if (!fmstr.SendCommand("AlignRotor()", cmdRes)) {
    alert("Error in jscript code!");
  }
}

function AdjustPosition()
{
  var bSucc, val;
  var MicroPos;
  var FullPos;

  bSucc = fmstr.ReadVariable("stepper.position");
  val = fmstr.LastVariable_vValue;
  
  //alert("New pos: " + val);
  
  if (bSucc) {
    if (val > 0) {
      FullPos = Math.floor(val / LastStepSize);
    }
    else {
      FullPos = Math.ceil(val / LastStepSize);
    }

    MicroPos = val % LastStepSize;
    
    if (FullPos == 1) {
      Temp = FullPos + " full-step";
    }
	else {
      Temp = FullPos + " full-steps";
	}
    
	
    if (MicroPos == 1) {
      Temp = Temp + ", " + MicroPos + " micro-step";
    }
    else if (MicroPos != 0) {
      Temp = Temp + ", " + MicroPos + " micro-steps";
    }


    $("#position").text(Temp);
  }
}

function AdjustStatus()
{
  var bSucc, val;

  bSucc = fmstr.ReadVariable("stepper.status");
  val = fmstr.LastVariable_vValue;
  
  if (bSucc) {
    if (val == 0) {
      $("#state").text("Motor is running");  
      /* Disable buttons. */
      DisableButtons(true);
      if ($("input[name='controlMode']:checked").val() == 'FULL_STEP') {
        /* Full-stepping. */
        if ($('#fullContinual').is(':checked') == true) {
          /* Continual enabled. */
          document.getElementById("stopBtn").disabled = false; 
        }
      }
      else {
        /* Micro-stepping. */
        if ($('#microContinual').is(':checked') == true) {
          /* Continual enabled. */
          document.getElementById("stopBtn").disabled = false; 
        }
      }
    }
    else if (val == 1) {
      $("#state").text("Motor is ready to run");
      /* Enable buttons */
      DisableButtons(false);    
      /* Disable stop button. */
      document.getElementById("stopBtn").disabled = true; 
    }
    else {
      $("#state").text("An error occurred");
    }
  }
}

function OnVariableChanged(bsVarName, vSubscriptionId)
{
  switch (bsVarName) {
    case "stepper.position":
      AdjustPosition();
      break;
    case "stepper.status":
      AdjustStatus();
      break;
    default:
      break;
  }
}

function PageInit()
{    
  var result
  
  /* Initialize GUI. */
  AdjustStatus();
  SetControlBox();
  /* Initialize speed label. */
  var actualSpeed = $("#fullSlider").slider("option", "value");
  $("#fullSpeedLabel").text(actualSpeed + ' Full-steps/s');
  /* Initialize speed label. */
  var actualSpeed = $("#microSlider").slider("option", "value");
  $("#microSpeedLabel").text(actualSpeed + ' Micro-steps/s');
  
  // Subscribe variables watched by FreeMaster
  fmstr.SubscribeVariable("stepper.position", 1000, result);
  fmstr.SubscribeVariable("stepper.status", 1000, result);
  
  // Attach listener to the events
  fmstr.attachEvent("OnVariableChanged", OnVariableChanged, true);
}

