package com.qdii.tatoparser;

public class Sentence {
	
	public String getLanguage()
	{
		return language;
	}
	
	public String getData()
	{
		return data;
	}
	
	public int getId()
	{
		return id;
	}
	
	/* ____________ ATTRIBUTES______________
	 * Their values is filled up by the NDK */
	private int id; //32 bits
	private String data;
	private String language;
}
