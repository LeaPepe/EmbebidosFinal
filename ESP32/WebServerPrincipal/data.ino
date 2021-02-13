float someDataToRead1 = 220.0f;
String readData1() {
  float t = someDataToRead1;
  if (isnan(t)) {
    Serial.println("Failed to read");
    return "";
  }
  else {
    //Serial.println(t);
    return String(t);
  }
}

float someDataToRead2 = 10.0f;
String readData2() {
  float h = someDataToRead2;
  if (isnan(h)) {
    Serial.println("Failed to read");
    return "";
  }
  else {
    //Serial.println(h);
    return String(h);
  }
}
