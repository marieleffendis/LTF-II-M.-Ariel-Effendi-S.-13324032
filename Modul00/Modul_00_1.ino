int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Sawit");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Saya sudah berjalan sebanyak: "); 
  Serial.print(counter);
  Serial.println(" kali"); 
  counter++;
  Serial.print("Saya sudah jalan selama: ");  
  Serial.print(millis());
  Serial.println(" milidetik");
  delay(3000);
}
