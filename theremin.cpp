/*****************************************************************************\
 * This file is part of LeapTheremin.                                        *
 *                                                                           *
 * LeapTheremin is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * LeapTheremin is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with LeapTheremin.  If not, see <http://www.gnu.org/licenses/>.     *
\*****************************************************************************/
#include <iostream>
#include <cmath>
#include "Leap.h"
#include "CsAudio.h"
#include "notas.h"

//This constants will modify controller behavior
//FLOOR sets the base height to measure volume. Defaults to 100 milimeters
//FREQ_M sets frequence multiplier. Higher values will increase theremin's "sensitivity"
//VOLUME_M sets volume multipliyer. Have in mind that valid volume range goes from 0.0 to 1.0 
#define THMN_FLOOR 200
#define THMN_FREQ_M_X 12
#define THMN_VOLUME_M 20

using namespace Leap;

class ThereminController : public Listener {
  private:
    AudioLib* audio;
    double freq;
    double volume;
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    void setAudio(AudioLib* audio);
    ThereminController();
    void play();
};

ThereminController::ThereminController() : Listener() {
  freq=440.0;
  volume=0.001;
  audio=NULL;
  std::cout << "Creating controller" << std::endl;
}

void ThereminController::setAudio(AudioLib* audio) {
  this->audio=audio;
}

void ThereminController::play(){
  if(audio)
  	audio->play(freq,volume);
}

void ThereminController::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void ThereminController::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
}

void ThereminController::onDisconnect(const Controller& controller) {
  //Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void ThereminController::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

void ThereminController::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();

  InteractionBox iBox = frame.interactionBox();

  if (frame.hands().count()>=2) {
    // Get the first hand
    const Hand vHand = frame.hands().leftmost();
    const Hand pHand = frame.hands().rightmost();

    Leap::Vector vPoint=vHand.palmPosition();
    Leap::Vector vPointN=iBox.normalizePoint(vPoint, true);

    volume=vPointN.y*30000-1000;



    double x=0,y=0,z=0;
    Leap::Vector leapPoint=pHand.palmPosition();
    Leap::Vector normalizedPoint=iBox.normalizePoint(leapPoint, false);
    x=normalizedPoint.x;
    y=pHand.palmPosition().y;
    z=pHand.palmPosition().z;

	//volume=(vHand.palmPosition().y-THMN_FLOOR)*THMN_VOLUME_M;

	freq=4000 - abs(normalizedPoint.y*3500);

	std::cout << "X: " << x << " Y: " << y << " Z: " << z << "volume: " << volume  << "freq: " << freq << std::endl;

	if(volume<0) volume=0;
	if(freq<0) freq=0;
	play();

  } else  {
      volume=0;
      freq=0;
      play();
  }
}

void ThereminController::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void ThereminController::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

ThereminController listener;

int main(int argc,const char** argv) {

  CsAudio audio;
  Controller controller;
  audio.initializeAudio(argc,argv);
  listener.setAudio(&audio);

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  // Keep this process running until Enter is pressed
  std::cout << "Press any key to exit... " << std::endl;
  std::cin.get();

  // Remove the sample listener when done
  controller.removeListener(listener);

  return 0;
}
