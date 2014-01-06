//-----------------------------------------------------------------------------
// Entaro ChucK Developer!
// This is a Chugin boilerplate, generated by chugerate!
//-----------------------------------------------------------------------------

// this should align with the correct versions of these ChucK files
#include "chuck_dl.h"
#include "chuck_def.h"
#include "elldefs.h"

// general includes
#include <stdio.h>
#include <limits.h>

#define MAX(a,b) ( (a)>(b)?(a):(b) )
#define MIN(a,b) ( (a)<(b)?(a):(b) )

// declaration of chugin constructor
CK_DLL_CTOR(elliptic_ctor);
// declaration of chugin desctructor
CK_DLL_DTOR(elliptic_dtor);

// example of getter/setter
CK_DLL_MFUN(elliptic_setParam);
CK_DLL_MFUN(elliptic_getParam);

CK_DLL_MFUN(elliptic_setRipple);
CK_DLL_MFUN(elliptic_getRipple);

CK_DLL_MFUN(elliptic_setAtten);
CK_DLL_MFUN(elliptic_getAtten);

CK_DLL_MFUN(elliptic_setBypass);
CK_DLL_MFUN(elliptic_getBypass);

CK_DLL_MFUN(elliptic_lop);
CK_DLL_MFUN(elliptic_hip);
CK_DLL_MFUN(elliptic_bp);

// for Chugins extending UGen, this is mono synthesis function for 1 sample
CK_DLL_TICKF(elliptic_tick);

// this is a special offset reserved for Chugin internal data
t_CKINT elliptic_data_offset = 0;

extern "C" {
  extern double ellset(double f1, double f2, double f3,
		       double ripple, double atten, double srate);
  extern int get_nsections(void);
  extern int ellpset(EllSect [], float *);
  
  extern float ellipse(float, int, EllSect [], float);
}

// class definition for internal Chugin data
// (note: this isn't strictly necessary, but serves as example
// of one recommended approach)
class Elliptic
{
public:
  // constructor
  Elliptic( t_CKFLOAT fs)
  {
	bypass = false;
    nsects = 0;
    srate = fs;
    ripple = 0.2;
    atten = 90;
    f1 = 400;
    f2 = 800;
    f3 = 0;
  }

  ~Elliptic()
  {
	es[0] = NULL;
	es[1] = NULL;
	delete [] es[0];
	delete [] es[1];
  }
  
  // for Chugins extending UGen
  void tick( SAMPLE* in, SAMPLE* out, int nframes)
  {
    // default: this passes whatever input is patched into Chugin
	memset (out, 0, sizeof(SAMPLE)*2*nframes);
	
	if (nsects==0 || bypass)
	  {
		for (int i=0; i<nframes; i++)
		  {
			out[i] = in[i];
			out[i+1] = in[i+1];
		  }
	  }
	else
	  {
		for (int i=0; i<nframes; i++)
		  {
			out[i] = ellipse(in[i], nsects, es[0], xnorm);
			out[i+1] = ellipse(in[i+1], nsects, es[1], xnorm);
		  }
	  }
  }
  
  float setRipple ( t_CKFLOAT p)
  {
    ripple = p;
    ellset (f1, f2, f3, ripple, atten, srate);
    return p;
  }

  float setAtten ( t_CKFLOAT p)
  {
    atten = p;
    ellset (f1, f2, f3, ripple, atten, srate);
    return p;
  }

  int setBypass ( t_CKINT p)
  {
	int val = (int)p;
	if (val > 0)
	  {
		bypass = true;
		printf ("Elliptic: bypass on\n");
	  }
	else
	  {
		bypass = false;
		printf ("Elliptic: turning off bypass\n");
	  }		
	return p;
  }

  int getBypass () { return bypass ? 1 : 0; }

  float getRipple() { return ripple; }
  float getAtten() { return atten; }

  void lowPass ( t_CKFLOAT p, t_CKFLOAT s)
  {
    f1 = MIN(p,s);
    f2 = MAX(p,s);
    f3 = 0;
    initialize_filter();
  }

  void highPass ( t_CKFLOAT p, t_CKFLOAT s)
  {
    f1 = MAX(p,s);
    f2 = MIN(p,s);
    f3 = 0;
    initialize_filter();
  }

  void bandPass ( t_CKFLOAT p, t_CKFLOAT q, t_CKFLOAT r )
  {
    f1 = MIN(p,q);
    f2 = MAX(p,q);
    f3 = r;
    initialize_filter();
  }

private:

  void initialize_filter ()
  {
    ellset (f1, f2, f3, ripple, atten, srate);
    nsects = get_nsections();
	es[0] = new EllSect[nsects];
    ellpset(es[0], &xnorm);
	es[1] = new EllSect[nsects];
    ellpset(es[1], &xnorm);
    if (nsects == 0)
      printf("Elliptic: filter not yet initialized. (You may need to relax the specs.)\n");
    else
      printf("Elliptic: filter initialized.\n");
  }
    
  // instance data
  bool bypass;
  double f1, f2, f3;
  float atten, ripple;
  float xnorm;
  float srate;
  int nsects;
  EllSect *es[2];
};


// query function: chuck calls this when loading the Chugin
// NOTE: developer will need to modify this function to
// add additional functions to this Chugin
CK_DLL_QUERY( Elliptic )
{
  // hmm, don't change this...
  QUERY->setname(QUERY, "Elliptic");
  
  // begin the class definition
  // can change the second argument to extend a different ChucK class
  QUERY->begin_class(QUERY, "Elliptic", "UGen");
  
  // register the constructor (probably no need to change)
  QUERY->add_ctor(QUERY, elliptic_ctor);
  // register the destructor (probably no need to change)
  QUERY->add_dtor(QUERY, elliptic_dtor);
  
  // for UGen's only: add tick function
  QUERY->add_ugen_funcf(QUERY, elliptic_tick, NULL, 2, 2);
  
  // NOTE: if this is to be a UGen with more than 1 channel, 
  // e.g., a multichannel UGen -- will need to use add_ugen_funcf()
  // and declare a tickf function using CK_DLL_TICKF
  
  // example of adding setter method
  QUERY->add_mfun(QUERY, elliptic_setRipple, "float", "ripple");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "float", "arg");

  // example of adding setter method
  QUERY->add_mfun(QUERY, elliptic_setAtten, "float", "atten");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "float", "arg");
  
  // example of adding getter method
  QUERY->add_mfun(QUERY, elliptic_getRipple, "float", "ripple");
  // example of adding getter method
  QUERY->add_mfun(QUERY, elliptic_getAtten, "float", "atten");

  // example of adding setter method
  QUERY->add_mfun(QUERY, elliptic_setBypass, "int", "bypass");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "int", "arg");
  
  // example of adding getter method
  QUERY->add_mfun(QUERY, elliptic_getBypass, "int", "bypass");

  // example of adding setter method
  QUERY->add_mfun(QUERY, elliptic_lop, "void", "lpf");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "float", "stop");
  QUERY->add_arg(QUERY, "float", "pass");

  // example of adding setter method
  QUERY->add_mfun(QUERY, elliptic_hip, "void", "hpf");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "float", "stop");
  QUERY->add_arg(QUERY, "float", "pass");

  // example of adding setter method
  QUERY->add_mfun(QUERY, elliptic_hip, "void", "bpf");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "float", "stop");
  QUERY->add_arg(QUERY, "float", "hip");
  QUERY->add_arg(QUERY, "float", "lop");
  
  // this reserves a variable in the ChucK internal class to store 
  // referene to the c++ class we defined above
  elliptic_data_offset = QUERY->add_mvar(QUERY, "int", "@e_data", false);
  
  // end the class definition
  // IMPORTANT: this MUST be called!
  QUERY->end_class(QUERY);
  
  // wasn't that a breeze?
  return TRUE;
}


// implementation for the constructor
CK_DLL_CTOR(elliptic_ctor)
{
  // get the offset where we'll store our internal c++ class pointer
  OBJ_MEMBER_INT(SELF, elliptic_data_offset) = 0;
  
  // instantiate our internal c++ class representation
  Elliptic * bcdata = new Elliptic(API->vm->get_srate());
  
  // store the pointer in the ChucK object member
  OBJ_MEMBER_INT(SELF, elliptic_data_offset) = (t_CKINT) bcdata;
}


// implementation for the destructor
CK_DLL_DTOR(elliptic_dtor)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // check it
  if( bcdata )
    {
      // clean up
      delete bcdata;
      OBJ_MEMBER_INT(SELF, elliptic_data_offset) = 0;
      bcdata = NULL;
    }
}


// implementation for tick function
CK_DLL_TICKF(elliptic_tick)
{
  // get our c++ class pointer
  Elliptic * c = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  
  // invoke our tick function; store in the magical out variable
  //if(c) *out = c->tick(in);
  if(c) c->tick(in, out, nframes);

  // yes
  return TRUE;
}


// example implementation for setter
CK_DLL_MFUN(elliptic_setRipple)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  RETURN->v_float = bcdata->setRipple(GET_NEXT_FLOAT(ARGS));
}

// example implementation for setter
CK_DLL_MFUN(elliptic_setAtten)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  RETURN->v_float = bcdata->setAtten(GET_NEXT_FLOAT(ARGS));
}

// example implementation for getter
CK_DLL_MFUN(elliptic_getRipple)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  RETURN->v_float = bcdata->getRipple();
}

// example implementation for getter
CK_DLL_MFUN(elliptic_getAtten)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  RETURN->v_float = bcdata->getAtten();
}

// example implementation for setter
CK_DLL_MFUN(elliptic_setBypass)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  RETURN->v_int = bcdata->setBypass(GET_NEXT_INT(ARGS));
}

// example implementation for getter
CK_DLL_MFUN(elliptic_getBypass)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  RETURN->v_int = bcdata->getBypass();
}

// example implementation for getter
CK_DLL_MFUN(elliptic_lop)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  t_CKFLOAT stop = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT pass = GET_NEXT_FLOAT(ARGS);
  bcdata->lowPass(pass,stop);
}

// example implementation for getter
CK_DLL_MFUN(elliptic_hip)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  t_CKFLOAT stop = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT pass = GET_NEXT_FLOAT(ARGS);
  bcdata->highPass(pass,stop);
}

// example implementation for getter
CK_DLL_MFUN(elliptic_bp)
{
  // get our c++ class pointer
  Elliptic * bcdata = (Elliptic *) OBJ_MEMBER_INT(SELF, elliptic_data_offset);
  // set the return value
  t_CKFLOAT stop = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT hip = GET_NEXT_FLOAT(ARGS); 
  t_CKFLOAT lop = GET_NEXT_FLOAT(ARGS);
  bcdata->bandPass(lop, hip, stop);
}