class Button{
  int _buttonPin;
  int _prevButtonClickedState;
  int _prevButtonReleasedState;
  bool _pullUp;

  public:

    //Constructor//
    Button(int buttonPin, bool pullUp = true){
      _buttonPin = buttonPin;
      _pullUp = pullUp;

      if(pullUp){
        pinMode(_buttonPin, INPUT_PULLUP);
      }
    }

    ////Button Functions////

    //Checks if button is currently pressed down
    bool isPressed(){
      bool _buttonState;

      _buttonState = digitalRead(_buttonPin);
      if(_pullUp){
        _buttonState = !_buttonState;
      }

      return _buttonState;
    }

    //Checks for the first moment the button is clicked
    bool isClicked(){
      bool _buttonState, _clicked;

      //Read pin and invert it if pullup is set 
      _buttonState = digitalRead(_buttonPin);
      if(_pullUp){
        _buttonState = !_buttonState;
      }

      //Check for a change in state
      if(_buttonState != _prevButtonClickedState){  //Is new state different?
        _clicked = _buttonState;    //_clicked sets to current button state- true if rising edge, false if falling edge
      } else{
        _clicked = false;           //if current state and previous state are same, return false
      }

      _prevButtonClickedState = _buttonState;

      return _clicked;
    }

    bool isReleased(){
      bool _buttonState, _released;

      //Read pin and invert it if pullup is set 
      _buttonState = digitalRead(_buttonPin);
      if(_pullUp){
        _buttonState = !_buttonState;
      }

      //Check for a change in state
      if(_buttonState != _prevButtonReleasedState){   //Is new state different?
        _released = _prevButtonReleasedState;   //_released sets to last state - basically the opposite of above
      } else{
        _released = false;
      }

      _prevButtonReleasedState = _buttonState;
      return _released;
    }

};