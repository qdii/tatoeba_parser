package com.qdii.tatoparser;

import android.os.Bundle;
import android.app.Activity;
import android.content.DialogInterface;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.AdapterView.OnItemSelectedListener;

public class ActivitySearch extends Activity {
	
	native void tatoparser_init();
	native void tatoparser_terminate();
	
	static {
		System.loadLibrary("tatoparser");
	}
	
	private String selectedLanguage = null;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_search);
        
        // fill up the drop-down list with the language names 
        ArrayAdapter<CharSequence> languageAdapter = ArrayAdapter.createFromResource(this,
                R.array.languages, android.R.layout.simple_spinner_item);
        ((Spinner) findViewById(R.id.dropLanguageList)).setAdapter( languageAdapter );
        
        // update attribute when the user clicks on an item of the language list 
        ((Spinner) findViewById(R.id.dropLanguageList)).setOnItemSelectedListener( 
        		new OnItemSelectedListener() {
					@Override
					public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
						selectedLanguage = (String) parent.getItemAtPosition(position);
					}

					@Override
					public void onNothingSelected(AdapterView<?> parent) {
						selectedLanguage = "auto";
					}
        		}
        );
        
        // start the library
        tatoparser_init();
    }
    

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
}
