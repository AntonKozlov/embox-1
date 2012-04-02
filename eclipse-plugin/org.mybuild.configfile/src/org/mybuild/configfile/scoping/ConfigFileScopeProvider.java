/*
 * generated by Xtext
 */
package org.mybuild.configfile.scoping;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.xtext.scoping.IScope;
import org.mybuild.configfile.configFile.ModuleReferenceWithInitializer;
import org.mybuild.myfile.Module;
import org.mybuild.myfile.PropertyValueBinding;
import org.mybuild.myfile.scoping.MyFileScopeProvider;

/**
 * This class contains custom scoping description.
 * 
 * see : http://www.eclipse.org/Xtext/documentation/latest/xtext.html#scoping on
 * how and when to use it
 * 
 */
public class ConfigFileScopeProvider extends MyFileScopeProvider {

	public IScope scope_PropertyValueBinding_property(PropertyValueBinding ctx,
			EReference ref) {
		EObject container = ctx.eContainer();
		if (container == null) {
			throw new IllegalStateException(
					"context.eContainer may not be null");
		}

		if (container instanceof ModuleReferenceWithInitializer) {
			ModuleReferenceWithInitializer moduleInitializer = (ModuleReferenceWithInitializer) container;
			Module module = moduleInitializer.getType();
			if (module == null || module.eIsProxy()) {
				return IScope.NULLSCOPE;
			}

			return createPropertiesScope(module.getOptions());
		}

		return super.scope_PropertyValueBinding_property(ctx, ref);
	}

}
