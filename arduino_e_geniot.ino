void sendToGeniot(char* shortcut, float 
); 
void decodificar(void);

/*
 * Definição dos Sensores utilizados no programa.
 */
#define Potenciometro "S00" 
#define led "S01"

/*
 * Variáveis auxiliares do programa.
 */
String inputString = "";       
boolean stringComplete = false;  
int Led = 10;

/*
 * Configuração inicial.
 */
void setup() {
  
  pinMode(Led, OUTPUT);          
  digitalWrite(Led, LOW); 
  Serial.begin(9600);
  Serial.write("geniot PRO Industrial\r\n");
  /*
   * Reserva 20 bytes devido ao retorno da string fornecida pelo geniot.
   */
  inputString.reserve(20);
  
}

void loop() {

   static const long interval = 10000;  
   static unsigned long previousMillis = 0, currentMillis;
   
   currentMillis = millis();

   int dados = analogRead(0);

   /*
    * A cada 10 segundos envia os valores do Potenciometro.
    */

   if(currentMillis - previousMillis >= interval) 
    {
       previousMillis = currentMillis;

       /*
        * Envia para o software geniot (PC) via Serial valores das variáveis
        */
       sendToGeniot(Potenciometro, dados);
     
    }
}

/*
 * Esta função é chamada quando é recebido algum caractere via interrupção serial.
 * O caractere coringa '\n' indica que todos os caracteres enviados pelo geniot (PC)
 * foram recebidos pelo MCU. Agora podemos decodificar/tratar/validar o pacote de dados.
 */
void serialEvent(void) {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
      decodificar();
    }
  }
}


void decodificar(void){
     if (stringComplete) 
     {
      /*
       * Verifica se o retorno é para a Tag Led! 
       * Caso seja, o Led é ligado ou desligado.
       */
        if(inputString[0] == '{' && inputString[1] == '\"'
                                 && inputString[2] == 'S'
                                 && inputString[3] == '0' 
                                 && inputString[4] == '1'
                                 && inputString[5] == '\"'
                                 && inputString[6] == ':')
        {
            if(inputString[8] == '}')
            {
              /*
               * Liga ou desliga o Led. Se o 7° byte for 1, liga o Led, caso seja 0, desliga o Led. 
               * Caso seja qualquer valor diferente de 1 e 0, mantém o último estado do Led (estado anterior).
               */
              digitalWrite(Led, (((inputString[7]-'0')==0)?0:1)); //ASC - '0' //aciona ou desaciona relé/led...etc
            }
          
        }
        /*
         * Zera as variáveis para tratar um novo retorno.
         */

        inputString = "";
        stringComplete = false;
     }
  }

/*
 * Esta função é responsável em formatar o valor a ser enviado para o geniot
 * conforme: "{S00:123.45}\r\n" .Este é o formato aceito pelo geniot PC.
 */
void sendToGeniot(char* shortcut, float value)
{
   Serial.print("{");
   Serial.print(shortcut);
   Serial.print(":");
   Serial.print(value);
   Serial.print("}\r\n"); 
}

