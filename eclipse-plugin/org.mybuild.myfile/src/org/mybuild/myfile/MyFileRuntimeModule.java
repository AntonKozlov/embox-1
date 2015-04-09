/*
 * generated by Xtext
 */
package org.mybuild.myfile;

import static com.google.inject.name.Names.named;

import org.eclipse.xtext.conversion.impl.STRINGValueConverter;
import org.eclipse.xtext.naming.IQualifiedNameProvider;
import org.eclipse.xtext.resource.IResourceServiceProvider;
import org.eclipse.xtext.scoping.IScopeProvider;
import org.eclipse.xtext.scoping.impl.AbstractDeclarativeScopeProvider;
import org.mybuild.myfile.conversions.TripleQuotedStringConverter;
import org.mybuild.myfile.naming.MyQualifiedNameProvider;
import org.mybuild.myfile.resource.FileExensionUnawareResourceServiceProvider;
import org.mybuild.myfile.scoping.MyImportedNamespaceAwareLocalScopeProvider;

/**
 * Use this class to register components to be used at runtime / without the
 * Equinox extension registry.
 */
public class MyFileRuntimeModule extends
		org.mybuild.myfile.AbstractMyFileRuntimeModule {

	public Class<? extends IResourceServiceProvider> bindIResourceServiceProvider() {
		return FileExensionUnawareResourceServiceProvider.class;
	}
	
	@Override
	public void configureIScopeProviderDelegate(com.google.inject.Binder binder) {
		binder.bind(IScopeProvider.class)
				.annotatedWith(
						named(AbstractDeclarativeScopeProvider.NAMED_DELEGATE))
				.to(MyImportedNamespaceAwareLocalScopeProvider.class);
	}

	@Override
	public Class<? extends IQualifiedNameProvider> bindIQualifiedNameProvider() {
		return MyQualifiedNameProvider.class;
	}

	public Class<? extends STRINGValueConverter> bindSTRINGValueConverter() {
		return TripleQuotedStringConverter.class;
	}

}