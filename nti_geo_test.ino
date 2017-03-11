#include <Servo.h>

#define HC12_SetupPin 4
#define GEO_CHANEL "AT+C001"
#define SAT_CHANEL "AT+C010"

String str;

Servo left_whel;
Servo right_whel;

struct GEO {
  int x;
  int y;
  int grad;
};

GEO geo_data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.flush();

  pinMode(HC12_SetupPin, OUTPUT);
  digitalWrite(HC12_SetupPin, HIGH);

  left_whel.attach(9);
  right_whel.attach(10);

  left_whel.write(90);
  right_whel.write(90);

  set_transmiter_chanel(GEO_CHANEL);

}

void loop() {

  //Test programm form movment via rectangle

  rotate_right_to(360);

  move_x(400);

  move_sat(25);

  move_x(400);

  rotate_right_to(90);

  move_y(400);

  rotate_right_to(180);

  move_x(-400);

  move_sat(-25);

  move_x(-400);

  rotate_right_to(270);

  move_y(-400);

}

void move_sat(int steps) {

  set_transmiter_chanel(SAT_CHANEL);
  //STOP
  Serial1.write((byte) 0x00); delay(50); //first byte broken at radio
  Serial1.write((byte) 0x30); delay(50); // 0
  Serial1.println();
  delay(1000);

  Serial.println(String(steps));
  Serial1.println(String(steps));

  delay(1000);

  while (1) {
    if (Serial1.available() > 0) {

      str = Serial1.readStringUntil('\r'); //have /r/n sting
      Serial.println(str);
      if (str == "STOP") {
        set_transmiter_chanel(GEO_CHANEL);
        read_geo_data();//wait until geo data normal
        return;
      }

    }
  }


}

void move_x(int steps) {

  GEO start_geo_data = read_geo_data();

  int speed = 30;

  right_whel.write(90 + speed);
  left_whel.write(90 - speed);

  while (1) {

    geo_data = read_geo_data();

    if (steps > 0 and geo_data.x > start_geo_data.x + steps) {
      servo_stop();
      return;
    }

    if (steps < 0 and geo_data.x < start_geo_data.x + steps) {
      servo_stop();
      return;
    }
  }

}

void move_y(int steps) {

  GEO start_geo_data = read_geo_data();

  int speed = 30;

  right_whel.write(90 + speed);
  left_whel.write(90 - speed);

  while (1) {

    geo_data = read_geo_data();

    if (steps > 0 and geo_data.y > start_geo_data.y + steps) {
      servo_stop();
      return;
    }

    if (steps < 0 and geo_data.y < start_geo_data.y + steps) {
      servo_stop();
      return;
    }
  }

}


void rotate_right_to(int grad) {

  int speed = 8;

  while (1) {
    geo_data = read_geo_data();

    whels_speed(-20, 20, 100);

    //aprox grad
    //TODO have problem near 360->0 position
    if (geo_data.grad >= grad - 4 and geo_data.grad <= grad + 4) {
      servo_stop();
      Serial.println("ROTATE END");
      return;
    }
  }

}

void servo_stop() {

  left_whel.write(90);
  right_whel.write(90);
}

void whels_speed(int speed_l, int speed_r, int timeout) {

  left_whel.write(90 + speed_l);
  right_whel.write(90 - speed_r);

  delay(timeout);
  servo_stop();

}


GEO read_geo_data() {

  while (1) {

    if (Serial1.available() > 0) {

      str = Serial1.readStringUntil('\n');
      //str = "00;1019;0609;192";
      Serial.println(str);

      geo_data.x    = str.substring(3, 7).toInt();
      geo_data.y    = str.substring(8, 12).toInt();
      geo_data.grad = str.substring(13, 16).toInt();


      return geo_data;
    }
  }
}

void set_transmiter_chanel(char* set_chanel) {

  digitalWrite(HC12_SetupPin, LOW);
  delay(500);
  Send_command(set_chanel);

  digitalWrite(HC12_SetupPin, HIGH);

}

void Send_command(char* ATcommand) {

  Serial1.println (ATcommand);
  Serial.println (ATcommand);
  delay(1000);
  if (Serial1.available() > 0)
  {
    str = Serial1.readStringUntil('\n');// LF
    Serial.println (str);
  }
  delay(500);
}


