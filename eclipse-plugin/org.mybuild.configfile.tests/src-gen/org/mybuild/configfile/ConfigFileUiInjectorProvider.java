/*
* generated by Xtext
*/
package org.mybuild.configfile;

import org.eclipse.xtext.junit4.IInjectorProvider;

import com.google.inject.Injector;

public class ConfigFileUiInjectorProvider implements IInjectorProvider {
	
	public Injector getInjector() {
		return org.mybuild.configfile.ui.internal.ConfigFileActivator.getInstance().getInjector("org.mybuild.configfile.ConfigFile");
	}
	
}
