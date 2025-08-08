/*
 * Projeto: Ventilador Inteligente com Acionamento por Toque Duplo
 * Autor: Brunansantiago
 * Versão: 1.0
 * Data: 08/08/2025
 * Descrição:
 * Exige duas passadas de mão em um curto intervalo para ligar/desligar.
 * - Passe a mão 1x: O sistema "acorda" e espera pela confirmação.
 * - Passe a mão 2x (em até 4 seg): O ventilador liga ou desliga.
 */

//Pinos de Controle
const int PINO_SENSOR_IR = 8;
const int PINO_RELE = 7;

//Constantes de Tempo
//Define os tempos em um só lugar para facilitar ajustes futuros.
const long INTERVALO_MAXIMO_TOQUE = 4000; //4 segundos para dar o segundo toque
const int DEBOUNCE_DELAY = 300; //Pausa para evitar múltiplos gatilhos com uma única passada

//Variáveis de Controle de Estado
//Controlam o estado atual do sistema (ligado/desligado, esperando/não esperando).
bool ventiladorEstaLigado = false;
int ultimoEstadoSensor = HIGH;
bool esperandoSegundoToque = false;
unsigned long tempoPrimeiroToque = 0;

void setup(){
  Serial.begin(9600);
  pinMode(PINO_SENSOR_IR, INPUT);
  pinMode(PINO_RELE, OUTPUT);
  
  //Garante que o relé comece no estado "desligado".
  //HIGH = circuito aberto/desligado, depende do tipo de relé usado. 
  digitalWrite(PINO_RELE, HIGH); 
  
  Serial.println("Ventilador por Toque Duplo iniciado.");
  Serial.println("Aguardando o primeiro toque..."); 
}

void loop(){
  int estadoAtualSensor = digitalRead(PINO_SENSOR_IR);

  //Detecta a borda de descida (mão acabou de passar na frente do sensor).
  //Isso acontece apenas uma vez por passada, evitando leituras múltiplas.
  if(estadoAtualSensor == LOW && ultimoEstadoSensor == HIGH){ 
     if(!esperandoSegundoToque){
      //Este é o PRIMEIRO toque. O sistema agora espera pela confirmação.
        esperandoSegundoToque = true;
        tempoPrimeiroToque = millis(); //Guarda o momento exato do primeiro toque.
        Serial.println("Primeiro toque detectado. Aguardando confirmacao...");
    }else{
      //Este é o SEGUNDO toque, que confirma a ação.
        ventiladorEstaLigado = !ventiladorEstaLigado; // Inverte o estado atual do ventilador.
        if(ventiladorEstaLigado){
        Serial.println("Confirmado! --> LIGANDO o ventilador.");
        digitalWrite(PINO_RELE, LOW); //LOW fecha o circuito e liga o ventilador.
    }else{
        Serial.println("Confirmado! --> DESLIGANDO o ventilador.");
        digitalWrite(PINO_RELE, HIGH); //HIGH abre o circuito e desliga o ventilador.
    }   
      //Reseta o sistema para o estado inicial, aguardando um novo ciclo de toques.
        esperandoSegundoToque = false;
        Serial.println("Aguardando o proximo ciclo de toques...");
    } 
    //Pausa essencial para "debounce", ignorando reflexos ou leituras instáveis do sensor.
        delay(DEBOUNCE_DELAY); 
  }
  //Se o sistema estiver esperando pelo segundo toque e o tempo máximo passar,
  //a operação é cancelada para evitar um acionamento acidental mais tarde.
 if(esperandoSegundoToque && (millis() - tempoPrimeiroToque > INTERVALO_MAXIMO_TOQUE)){
        Serial.println("Tempo esgotado. Operacao cancelada.");
        esperandoSegundoToque = false; //Cancela a espera e volta ao estado inicial.
  }
  //Atualiza o estado do sensor para a próxima iteração do loop.
        ultimoEstadoSensor = estadoAtualSensor;
        delay(50); //Pequena pausa para estabilidade geral do loop.
  }
  }
