/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       * 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */

/* Use include guards,
just as you would with any regular header. */
#if !defined(MYPROJECT_PCH_H)
#define MYPROJECT_PCH_H

/* In addition, its a good idea to surround the headers
with an additional define so we can switch the inclusion
of the pre-compiled headers them on and off.
For platforms which don't support pre-compiled headers,
don't define USE_PRECOMPILED_HEADERS. */
#define USE_PRECOMPILED_HEADERS
#ifdef USE_PRECOMPILED_HEADERS

/* I like to include this pragma too,
so the build log indicates if pre-compiled headers
were in use. */
#pragma message( "Using pre-compiled headers\n" )

// Include files that are used in many places and change infrequently.

// Std library stuff
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <memory>

// Qt stuff
#include <qvector>
#include <qvariant>
#include <qlist>
#include <qstring>
#include <qstringlist>
#include <qmutex>
#include <qmutexlocker>
#include <qregexp>
#include <qmap>
#include <qabstractitemmodel>
#include <qmodelindex>
#include <QThreadStorage>
#include <qstandarditem>
#include <qstandarditemmodel>
#include <qthread>


#endif // USE_PRECOMPILED_HEADERS 

#endif //MYPROJECT_PCH_H.
