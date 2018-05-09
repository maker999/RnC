#include <LIS3MDLSensor.h>

#include <nist256p1.h>
#include <ripemd160.h>
#include <segwit_addr.h>
#include <script.h>
#include <blake2b.h>
#include <base58.h>
#include <blake2s.h>
#include <nem_serialize.h>
#include <sha3.h>
#include <address.h>
#include <check_mem.h>
#include <bip32.h>
#include <ecdsa.h>
#include <pbkdf2.h>
#include <options.h>
#include <base32.h>
#include <bip39_english.h>
#include <hasher.h>
#include <sha2.h>
#include <hmac.h>
#include <bip39.h>
#include <macros.h>
#include <rc4.h>
#include <rand.h>
#include <nem.h>
#include <curves.h>
#include <blake2_common.h>
#include <blake256.h>
#include <bignum.h>
#include <secp256k1.h>

#define I2C2_SCL    PB10
#define I2C2_SDA    PB11
TwoWire *dev_i2c;
LIS3MDLSensor *Magneto;

void hexify(const char *str, const uint8_t *hex, unsigned int len) {
  Serial.write(str);
  for (int i = 0; i < len; i++)  {
    static char tmp[4] = {};
    sprintf(tmp, "%02X", hex[i]);
    Serial.write(tmp);
  }
}

void entropogen(uint8_t *ent, unsigned int len){
  int32_t magnetometer[3];
  for (int i= 0 ; i < len ; i++){
    Magneto->GetAxes(magnetometer);
    *(ent+i) =  uint8_t((magnetometer[0] + magnetometer[1] + magnetometer[2]) *millis() ) % 32;
  }
      
}

void print_everything(){
  uint8_t ent[32];
 entropogen(ent,32);                     
 const char *mnemon = mnemonic_from_data(ent, 32);
  
  Serial.write(mnemon);
  

  uint8_t seed[512 / 8];
  const char *passphrase= "";
  mnemonic_to_seed(mnemon, passphrase, seed, NULL); 
  
  hexify("seed:", seed, 64);
  
  HDNode node, node_44h, node_1h, node_0h;

  hdnode_from_seed( seed, 64, SECP256K1_NAME, &node);

  hdnode_fill_public_key(&node);
  hexify("Node Chain:", node.chain_code, 32);

  hexify("Node Pub Key:", node.public_key, 33);

  char str[112];
  int strsize;
  
  hdnode_serialize_public(&node, 0x0000000, 0x043587DF, str, 112);

  hdnode_serialize_private(&node, 0x00000000, 0x0488ADE4, str, 112);

  Serial.print("Mainnet Private Key:");
  Serial.print(str);

  hdnode_serialize_private(&node, 0x00000000, 0x04358394, str, 112);

  hdnode_private_ckd_prime(&node, 44);
  uint32_t fp = hdnode_fingerprint(&node);
  hdnode_serialize_public(&node, fp, 0x04358394, str, 112);

  Serial.println(""); 
    
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Initialize LPS22HB Sensor
  dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
  dev_i2c->begin();
  Magneto = new LIS3MDLSensor(dev_i2c);
  Magneto->Enable();
   
}

void loop() {

  // put your main code here, to run repeatedly:
  
  int received = Serial.read();
  if(received == 50){
    
    print_everything();
    delay(1000);
  }
  
  
}



