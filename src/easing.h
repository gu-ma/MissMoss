//
//  easing.h
//  MissMoss
//
//  Created by Guillaume on 25.06.18.
//

#ifndef easing_h
#define easing_h

class Easing : public ofBaseApp {
    
public:
    // no easing; no acceleration
    float linear(float t) { return t; };
    // accelerating from zero velocity
    float easeInQuad(float t) { return t*t; };
    // decelerating to zero velocity
    float easeOutQuad(float t) { return t*(2-t); };
    // acceleration until halfway; then deceleration
    float easeInOutQuad(float t) { return (t<.5) ? 2*t*t : -1+(4-2*t)*t; };
    // accelerating from zero velocity
    float easeInCubic(float t) { return t*t*t; };
    // decelerating to zero velocity
    float easeOutCubic(float t) { return (t-1)*t*t+1; };
    // acceleration until halfway; then deceleration
    float easeInOutCubic(float t) { return (t<.5) ? 4*t*t*t : float(t-1)*(2*t-2)*(2*t-2)+1; };
    // accelerating from zero velocity
    float easeInQuart(float t) { return t*t*t*t; };
    // decelerating to zero velocity
    float easeOutQuart(float t) { return 1-(t-1)*t*t*t; };
    // acceleration until halfway; then deceleration
    float easeInOutQuart(float t) { return (t<.5) ? 8*t*t*t*t : 1-8*(t-1)*t*t*t; };
    // accelerating from zero velocity
    float easeInQuint(float t) { return t*t*t*t*t; };
    // decelerating to zero velocity
    float easeOutQuint(float t) { return 1+(t-1)*t*t*t*t; };
    // acceleration until halfway; then deceleration
    float easeInOutQuint(float t) { return (t<.5) ? 16*t*t*t*t*t : 1+16*(t-1)*t*t*t*t; };
};


#endif /* easing_h */
