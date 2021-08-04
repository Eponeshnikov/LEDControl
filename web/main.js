var Socket;
var brightness = document.getElementById("brightness");
var mode = document.getElementById("mode");
var speed = document.getElementById("speed");
var color = document.getElementById("idColor")

function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
    } : null;
  }
  
  function rgb2hsv (r, g, b) {
      let rabs, gabs, babs, rr, gg, bb, h, s, v, diff, diffc, percentRoundFn;
      rabs = r / 255;
      gabs = g / 255;
      babs = b / 255;
      v = Math.max(rabs, gabs, babs),
      diff = v - Math.min(rabs, gabs, babs);
      diffc = c => (v - c) / 6 / diff + 1 / 2;
      percentRoundFn = num => Math.round(num * 100) / 100;
      if (diff == 0) {
          h = s = 0;
      } else {
          s = diff / v;
          rr = diffc(rabs);
          gg = diffc(gabs);
          bb = diffc(babs);
  
          if (rabs === v) {
              h = bb - gg;
          } else if (gabs === v) {
              h = (1 / 3) + rr - bb;
          } else if (babs === v) {
              h = (2 / 3) + gg - rr;
          }
          if (h < 0) {
              h += 1;
          }else if (h > 1) {
              h -= 1;
          }
      }
      return {
          h: Math.round(h * 255),
          s: percentRoundFn(s * 255),
          v: percentRoundFn(v * 255)
      };
  }

function init() {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
}

function sendBrightness(){
    Socket.send("br"+brightness.value);
}

function sendMode(){
    Socket.send("md"+mode.value);
}

function sendSpeed(){
    Socket.send("sp"+speed.value);
}


function sendColor(){
    let rgb = hexToRgb(color.value);
    let hsv = rgb2hsv(rgb.r, rgb.g, rgb.b);
    Socket.send("hh"+hsv.h);
    Socket.send("ss"+hsv.s);
    Socket.send("vv"+hsv.v);
}

document.addEventListener('DOMContentLoaded', init);
brightness.addEventListener("input", sendBrightness);
mode.addEventListener("input", sendMode);
speed.addEventListener("input", sendSpeed);
color.addEventListener("input", sendColor);
