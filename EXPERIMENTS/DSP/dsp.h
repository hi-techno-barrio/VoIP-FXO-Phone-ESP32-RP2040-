
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t linear2alaw(int16_t pcm_val) {
  const uint16_t ALAW_CLIP = 32635;
  const uint8_t ALAW_BIAS = 0x84;
  int16_t mask;
  uint8_t sign, exponent, mantissa;

  if (pcm_val >= 0) {
    sign = 0;
  } else {
    sign = 0x80;
    pcm_val = -pcm_val;
  }

  if (pcm_val > ALAW_CLIP) {
    pcm_val = ALAW_CLIP;
  }

  pcm_val += (1 << 7) - 1;
  exponent = G711_EXPONENT_TABLE[(pcm_val >> 7) & 0x7f];
  mantissa = (pcm_val >> (exponent + 3)) & 0x0f;
  mask = (exponent << 4) | mantissa;
  mask |= ALAW_BIAS;
  mask ^= sign;

  return mask;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t linear2ulaw(int16_t pcm_val) {
  const uint16_t ULAW_CLIP = 32635;
  const uint8_t ULAW_BIAS = 0x84;
  uint8_t sign, exponent, mantissa;
  uint16_t uval;

  sign = (pcm_val >> 8) & 0x80;
  if (sign != 0) {
    pcm_val = -pcm_val;
  }
  if (pcm_val > ULAW_CLIP) {
    pcm_val = ULAW_CLIP;
  }
  pcm_val += (1 << 4) - 1;
  exponent = G711_EXPONENT_TABLE[(pcm_val >> 7) & 0x7f];
  mantissa = (pcm_val >> (exponent + 3)) & 0x0f;
  uval = ~(sign | (exponent << 4) | mantissa);
  uval &= 0xff;
  uval |= ULAW_BIAS;

  return uval;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int16_t alaw2linear(uint8_t a_val) {
  const uint16_t ALAW_BIAS = 0x84;
  int16_t t, seg;
  a_val ^= ALAW_BIAS;
  t = (a_val & 0x0f) << 4;
  seg = ((unsigned)a_val & 0x70) >> 4;
  switch (seg) {
    case 0:
      t += 8;
      break;
    case 1:
      t += 0x108;
      break;
    default:
      t += 0x108;
      t <<= seg - 1;
  }
  return ((a_val & 0x80) ? t : -t);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int16_t ulaw2linear(uint8_t u_val) {
  const uint16_t ULAW_BIAS = 0x84;
  uint8_t t;
  int16_t sign, exponent, mantissa, pcm_val;

  u_val = ~u_val;
  sign = (u_val & 0x80);
  exponent = (u_val >> 4) & 0x07;
  mantissa = u_val & 0x0F;
  pcm_val = ((mantissa << 3) | 0x84) << (exponent + 2);
  pcm_val = (sign == 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void G711_encode(short *pcm, unsigned char *g711, int n_samples, int encode_type) {
  int i;
  unsigned char code;
  int exponent;
  const uint8_t *table;

  if (encode_type == G711_ALAW) {
    table = G711_COMPAND_TABLE;
  } else {
    table = G711_EXPONENT_TABLE;
  }

  for (i = 0; i < n_samples; i++) {
    exponent = pcm[i] >> 8;
    if (exponent < 0) {
      exponent = ~(exponent ^ 0x7F);
    }
    exponent += 3;

    code = table[exponent];
    code |= ((pcm[i] >> ((code & 0x07) ^ 0x07)) & (0xFF >> (code & 0x07))) ^ (code == 0x55);

    if (encode_type == G711_ALAW) {
      code ^= 0x55;
    }

    g711[i] = code;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void G711_decode(unsigned char *g711, short *pcm, int n_samples, int decode_type) {
  int i;
  int exponent;
  const uint8_t *table;
  unsigned char code;

  if (decode_type == G711_ALAW) {
    table = ALAW_COMPAND_TABLE;
  } else {
    table = ULAW_COMPAND_TABLE;
  }

  for (i = 0; i < n_samples; i++) {
    if (decode_type == G711_ALAW) {
      code = g711[i] ^ 0x55;
    } else {
      code = g711[i];
    }

    exponent = table[code & 0x7F];
    pcm[i] = ((code & 0x80) ? ((0x7F << exponent) | ((code & 0x7F) << (exponent - 4)) | (1 << (exponent - 5))) : ((0x7F ^ (code & 0x7F)) << exponent) | (1 << (exponent - 1)));
    pcm[i] = ((code & 0x80) ? -pcm[i] : pcm[i]);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int16_t alaw_compress(int16_t pcm_value) {
  const uint16_t ALAW_MAX = 0xFFF;
  const uint16_t SEGMENTS = 8;
  const uint16_t BIAS = 132;

  uint16_t mask = 0x8000;
  int sign = (pcm_value & mask) ? 1 : 0;
  if (sign) {
    pcm_value = -pcm_value;
  }

  uint16_t exponent = 7;
  for (int16_t tmp = pcm_value >> 4; tmp > 0; tmp >>= 1) {
    exponent--;
  }

  uint16_t mantissa = (pcm_value >> ((exponent == 7) ? 0 : (exponent + 3))) & 0x0F;
  uint16_t compressed = (sign << 7) | ((exponent + BIAS) << 4) | mantissa;
  compressed ^= 0x55;

  if (compressed > ALAW_MAX) {
    compressed = ALAW_MAX;
  }

  return static_cast<int16_t>(compressed);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int16_t ulaw_compress(int16_t pcm_value) {
  const uint16_t UMAX = 0xFF;
  const uint16_t BIAS = 0x84;
  const uint16_t CLIP = 32635;

  uint16_t mask = 0x8000;
  int sign = (pcm_value & mask) ? 1 : 0;
  if (sign) {
    pcm_value = ~pcm_value;
  }

  if (pcm_value > CLIP) {
    pcm_value = CLIP;
  }

  pcm_value += BIAS;

  uint16_t exponent = 7;
  for (int16_t tmp = pcm_value >> 7; tmp > 1; tmp >>= 1) {
    exponent--;
  }

  uint16_t mantissa = (pcm_value >> (exponent + 3)) & 0x0F;
  uint16_t compressed = ~(sign << 7 | (exponent << 4) | mantissa);

  if (compressed > UMAX) {
    compressed = UMAX;
  }

  return static_cast<int16_t>(compressed);
}

int16_t ulaw_decode(uint8_t ulaw) {
  ulaw = ~ulaw;

  int16_t sign = (ulaw & 0x80) ? -1 : 1;
  int16_t exponent = (ulaw >> 4) & 0x07;
  int16_t mantissa = ulaw & 0x0F;

  int16_t sample = (mantissa << 4) | 0x08;
  sample = sample << (exponent + 3);
  sample *= sign;

  return sample;
}


int16_t alaw_decode(uint8_t alaw) {
  alaw ^= 0x55;

  int16_t sign = (alaw & 0x80) ? -1 : 1;
  int16_t exponent = ((alaw >> 4) & 0x07) + 2;
  int16_t mantissa = alaw & 0x0F;

  int16_t sample = (mantissa << exponent) | 0x0080;
  sample *= sign;

  return sample;
}


