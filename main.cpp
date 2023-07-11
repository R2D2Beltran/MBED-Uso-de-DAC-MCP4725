 #include "mbed.h"
 #include "MCP4725.h"

static BufferedSerial pc(USBTX,USBRX); //BufferedSerial para envio de mas de un caracter y UnBuffereSerial para trabajr solo con una

 //Create an MCP4725 object at the default address (ADDRESS_0)
 MCP4725 dac(PB_3, PB_10); //SDA,SCL
 AnalogIn sini(A2,3.3);
float i=0.0;
    float voltage=0;    //voltaje de potenciometro
    int part_Entera=0;  //parte entera de flotante
    int part_Decimal=0; //parte decimal de flotante
    char envio[11]; 

void float_to_int(float flotante){
        part_Entera=int(flotante);
        part_Decimal=int((flotante-part_Entera)*1000);
}

void send_to_portSerial(char send[11]){
     
    sprintf(send,"%d.%d\n\r",part_Entera,part_Decimal);     //Asignacion de la parte entera y la   parte decimal a la variable char envio 
    pc.write(send,11);  //envio de lectura del divisor de voltaje al puerto serial
    
}

 int main()
 {   pc.set_baud(115200);  //sini.set_reference_voltage(5.0);
     //Try to open the MCP4725
     if (dac.open()) {
         printf("Device detected!\n");

         //Wake up the DAC
         //NOTE: This might wake up other I2C devices as well!
         dac.wakeup();

         while (1) {
             //Generate a sine wave on the DAC
             for (float i = 0.0; i < 360.0; i += 1){
                 dac.write(0.5 * (sinf(i * 3.14159265 / 180.0) + 1));
                 //voltage=dac.read();
                 voltage=sini.read_voltage()*100;
                 float_to_int(voltage);
                 send_to_portSerial(envio);
                 
                 
                //ThisThread::sleep_for(100ms); 
             }
         
     }} else {
         error("Device not detected!\n");
     }
 }