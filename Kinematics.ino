
void kin_norm(float *x, float *y)
{
  float norm_value_x = fabsf(*x);
  float norm_value_y = fabsf(*y);
  
  if(norm_value_x > 1)
  {
    *x /= norm_value_x;
    *y /= norm_value_x;
  }
  if(norm_value_y > 1)
  {
    *x /= norm_value_y;
    *y /= norm_value_y;
  }
}

void kin_set(float x, float y)
{
  float val_a = 0;
  float val_b = 0;

  /* y is the forward speed, being the base value of both motors a and b */
  val_a = y;
  val_b = y;

  /* x adds to motor a and subtracts from motor b, causing a rotation */
  val_a += x;
  val_b -= x;

  /* now normalize to 1 if larger */
  kin_norm(&val_a, &val_b);

  /* set outputs, swapping etc will be done in that lower layer */
  lego_motor(0, val_a);
  lego_motor(1, val_b);
}

