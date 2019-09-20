   //Prof.WilliamJosé
  // inclusão de bibliotecas.    
  #include <Servo.h>    //  biblioteca de manipulação de servo motor.    
  #include <AFMotor.h> //   biblioteca de manipulação de motor DC.  
    
  //Definindo os pinos  
  #define trigPin A0 
  #define echoPin A1 
  #define BUZZER A2    
  AF_DCMotor motor1(1);    // motor1 ligado na L293 ao M1  
  AF_DCMotor motor2(2);    // motor2 ligado na L293 ao M2  

  int TempoGirar = 1;//tempo do robô girar em 45º com uma bateria de 9v.
  int distanciaObstaculo = 30; //distância para o robô parar e recalcular o melhor caminho
  int velocidadeMotores = 80; // velocidade que os motores funcionarão na bateria 9v. Para a bateria 9v a velocidade 80 é ideal
  Servo servo_ultra_sonico; // nomeando o servo motor    
  //variáveis  para o sensor ultrassonico
  long duracao;
  long distancia_cm=0;
  int minimumRange=5; //tempo de resposta do sensor
  int maximumRange=200;
    
  // executado na inicialização do Arduino    
  void setup(){    
    Serial.begin(9600); // inicializa a comunicação serial para mostrar dados     
    servo_ultra_sonico.attach(10);  // Define o mini servo motor ligado no pino digital 10.    
    pinMode(trigPin, OUTPUT); //define o pino TRIG como saída
    pinMode(echoPin, INPUT);  //define o pino ECHO como entrada 
    pinMode(BUZZER,OUTPUT);   // Define o pino do buzzer como saída   
    motor1.setSpeed(velocidadeMotores);// Velocidade dos motores ,a máxima é 255. 
    motor2.setSpeed(velocidadeMotores);     
    servo_ultra_sonico.write(90);   // O servo do sensor se inicia a 90 graus (meio)    
    rotacao_Parado;  //inica com os motores parados     
  }    
    
  // Função principal do Arduino    
  void loop(){    
    pensar(); //inicia a função pensar  
  }    
    
  // Chamando outras funções para definir o que o robô irá fazer  
  void pensar(){    
   reposicionaServoSonar(); //Servo para olhar a frente    
   int distancia = lerSonar(); // Ler o sensor de distância  
   Serial.print("distancia em cm: "); 
   Serial.println(distancia);   // Exibe no serial monitor 
   if (distancia > distanciaObstaculo) {  // Se a distância for maior que 20 cm  
     rotacao_Frente(); //Anda para frente   
   }else{   
     rotacao_Parado();  //para o robô  
     posicionaCarroMelhorCaminho(); //calcula o melhor caminho    
     pensar();    
   }   
  }  
    
  // Função de Leitura e Calcudo de distância do sensor ultrassônico    
  int lerSonar(){    
   digitalWrite(trigPin, LOW); //não envia som
   delayMicroseconds(2);
   digitalWrite(trigPin,HIGH); //envia som 
   delayMicroseconds(10);
   digitalWrite(trigPin,LOW); //não envia o som e espera o retorno do som enviado
   duracao = pulseIn(echoPin,HIGH); //Captura a duração em tempo do retorno do som.
   distancia_cm = duracao/56; //Calcula a distância
   delay(30);  
   return distancia_cm;             // Retorna a distância  
  }   
    
  // Função para calcular a distância do centro    
  int calcularDistanciaCentro(){    
   servo_ultra_sonico.write(90);    
   delay(15);//20   
   int leituraDoSonar = lerSonar();  // Ler sensor de distância  
   delay(500);   
   leituraDoSonar = lerSonar();   
   delay(500);   
   Serial.print("Distancia do Centro: "); // Exibe no serial  
   Serial.println(leituraDoSonar);   
   return leituraDoSonar;       // Retorna a distância  
  }    
    
  // Função para calcular a distância da direita    
  int calcularDistanciaDireita(){    
   servo_ultra_sonico.write(0);   
   delay(190);  //200
   int leituraDoSonar = lerSonar();   
   delay(500);   
   leituraDoSonar = lerSonar();   
   delay(500);   
   Serial.print("Distancia da Direita: ");  
   Serial.println(leituraDoSonar);   
   return leituraDoSonar;    
  }    
    
  // Função para calcular a distância da esquerda    
  int calcularDistanciaEsquerda(){    
   servo_ultra_sonico.write(180);   
   delay(190); //200 
   int leituraDoSonar = lerSonar();   
   delay(500);   
   leituraDoSonar = lerSonar();   
   delay(500);   
   Serial.print("Distancia Esquerda: ");  
   Serial.println(leituraDoSonar);   
   return leituraDoSonar;    
  }    
    
  // Captar distâncias lidas e calcular a melhor distância    
  char calculaMelhorDistancia(){    
   int esquerda = calcularDistanciaEsquerda();    
   int centro = calcularDistanciaCentro();    
   int direita = calcularDistanciaDireita();    
   reposicionaServoSonar();    
   int maiorDistancia = 0;   
   char melhorDistancia = '0';     
     
   if (centro > direita && centro > esquerda){    
     melhorDistancia = 'c';    
     maiorDistancia = centro;    
   }else   
   if (direita > centro && direita > esquerda){    
     melhorDistancia = 'd';    
     maiorDistancia = direita;    
   }else  
   if (esquerda > centro && esquerda > direita){    
     melhorDistancia = 'e';    
     maiorDistancia = esquerda;    
   }    
   if (maiorDistancia <= distanciaObstaculo) { //distância limite para parar o robô   
     rotacao_Re();    
     posicionaCarroMelhorCaminho();    
   }    
   reposicionaServoSonar();  
   return melhorDistancia;    
  }    
    
  // Girar o Carro para a melhor distância    
  void posicionaCarroMelhorCaminho(){    
   char melhorDist = calculaMelhorDistancia();     
   Serial.print("melhor Distancia em cm: ");  
   Serial.println(melhorDist);  
   if (melhorDist == 'c'){    
     pensar();    
   }else if (melhorDist == 'd'){    
     rotacao_Direita();    
   }else if (melhorDist == 'e'){    
     rotacao_Esquerda();     
   }else{    
     rotacao_Re();    
   }    
   reposicionaServoSonar();    
  }    
    
  // Função para deixar o sensor "olho" do robô no centro    
  void reposicionaServoSonar(){    
   servo_ultra_sonico.write(90);   
   delay(200);   
  }    
    
  // Fazer o carro parar    
  void rotacao_Parado()    
  {    
   Serial.println(" Motor: Parar ");
   motor1.run(RELEASE); // Motor para  
   motor2.run(RELEASE);  
  }    
    
  // Fazer Robô andar para frente    
  void rotacao_Frente()    
  {    
   Serial.println("Motor: Frente ");   
   motor1.run(FORWARD); // Roda vai para frente  
   motor2.run(FORWARD);   
   delay(45);//50
  }    
    
  // Andar para trás e emitir na ré    
  void rotacao_Re()    
  {    
   Serial.println("Motor: ré ");  
   for (int i=0; i <= 3; i++){
      digitalWrite(BUZZER, HIGH); // Liga o som
      delay(100);
      motor1.run(BACKWARD);    // Roda vai para trás  
      motor2.run(BACKWARD);    // Roda vai para trás  
      delay(100);  
      digitalWrite(BUZZER, LOW); // Desliga o som  
      delay(100);
   } 
   rotacao_Parado();    
  }    
    
  // Virar à direita    
  void rotacao_Direita()    
  {    
   digitalWrite(BUZZER, HIGH); // Liga o som
   delay(100);
   motor1.run(BACKWARD);    //Dar uma ré para não colidir ao girar 
   motor2.run(BACKWARD);      
   delay(45);  //50
   digitalWrite(BUZZER, LOW); // Desliga o som  
   delay(100);
   Serial.println(" Para a direita ");  
   motor1.run(FORWARD); // Roda vai para frente  
   motor2.run(BACKWARD); // Roda vai para trás   
   delay(TempoGirar);    
  }    
    
  // Virar à esquerda    
  void rotacao_Esquerda()    
  {    
   digitalWrite(BUZZER, HIGH); // Liga o som
   delay(100);
   motor1.run(BACKWARD);    //Ré para não colidir ao girar 
   motor2.run(BACKWARD);   
   delay(45);//50  
   digitalWrite(BUZZER, LOW); // Desliga o som  
   delay(100);
   Serial.println(" Para a esquerda ");  
   motor1.run(BACKWARD); // Roda vai para trás  
   motor2.run(FORWARD); // Roda vai para frente  
   delay(TempoGirar);    
  }  
