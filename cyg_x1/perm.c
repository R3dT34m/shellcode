



void init(ctx_t *c, int mdlen)
{
    int i;

    for (i = 0; i < 25; i++)
        c->st.q[i] = 0;
    c->mdlen = mdlen;
    c->rsiz = 32 - 2 * mdlen;
    c->idx = 0;
}


void xof(ctx_t *c)
{
    c->st.b[c->idx] ^= 0x1F;
    c->st.b[c->rsiz - 1] ^= 0x80;
    permute(c->st.q);
    c->pt = 0;
}

void permute(w256_t *state, int rnds)
{
    int      i;
    uint64_t *s=(uint64_t*)&state->q[0];
    
    for (i=0; i<rnds; i++)
    {  
      s[0]+= s[1];                                                              
      s[1] = ROTL64(s[1], 13);                                                     
      s[1]^= s[0];                                                              
      s[0] = ROTL64(s[0], 32);                                                     
      s[2]+= s[3];                                                              
      s[3] = ROTL64(s[3], 16);                                                     
      s[3]^= s[2];                                                              
      s[0]+= s[3];                                                              
      s[3] = ROTL64(s[3], 21);                                                     
      s[3]^= s[0];                                                              
      s[2]+= s[1];                                                              
      s[1] = ROTL64(s[1], 17);                                                     
      s[1]^= s[2];                                                              
      s[2] = ROTL64(s[2], 32); 
    }
}
  