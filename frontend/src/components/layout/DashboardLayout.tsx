import React, { useState } from 'react';
import { Outlet, Link, useLocation } from 'react-router-dom';
import { useAuth } from '../../context/AuthContext';
import { motion, AnimatePresence } from 'framer-motion';
import { LogOut, Bell, Menu, X, FileText, CheckCircle, Settings, Users, LayoutDashboard } from 'lucide-react';

const DashboardLayout: React.FC = () => {
  const { user, logout } = useAuth();
  const location = useLocation();
  const [isMobileMenuOpen, setIsMobileMenuOpen] = useState(false);

  // Define navigation links based on user role
  const getNavLinks = () => {
    const role = user?.role.toUpperCase();
    
    const baseLinks = [
      { path: `/${role?.toLowerCase()}/dashboard`, label: 'Dashboard', icon: LayoutDashboard }
    ];

    if (role === 'AUTHOR') {
      return [
        ...baseLinks,
        { path: '/author/submissions', label: 'My Papers', icon: FileText }
      ];
    } else if (role === 'REVIEWER') {
      return [
        ...baseLinks,
        { path: '/reviewer/assignments', label: 'Assignments', icon: CheckCircle }
      ];
    } else if (role === 'ORGANIZER' || role === 'ADMIN') {
      return [
        ...baseLinks,
        { path: '/admin/submissions', label: 'All Submissions', icon: FileText },
        { path: '/admin/users', label: 'Users', icon: Users },
        { path: '/admin/settings', label: 'Settings', icon: Settings }
      ];
    }
    return baseLinks;
  };

  const navLinks = getNavLinks();

  return (
    <div className="min-h-screen bg-transparent text-white flex flex-col font-sans selection:bg-blue-500/30">
      {/* Top Navigation */}
      <nav className="sticky top-0 z-50 bg-[#0f172a]/60 backdrop-blur-xl border-b border-white/10 shadow-lg">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between items-center h-20">
            <div className="flex items-center">
              <Link to="/" className="flex-shrink-0 flex items-center space-x-3 group">
                <div className="w-10 h-10 rounded-xl bg-gradient-to-tr from-blue-600 to-indigo-600 flex items-center justify-center shadow-lg shadow-blue-500/20 group-hover:shadow-blue-500/40 transition-all duration-300 transform group-hover:rotate-6">
                  <span className="font-bold text-white text-base">CM</span>
                </div>
                <span className="font-bold text-xl tracking-tight bg-gradient-to-r from-white via-white to-slate-400 bg-clip-text text-transparent hidden sm:block">
                  Conference Manager
                </span>
              </Link>
              
              {/* Desktop Nav */}
              <div className="hidden md:ml-12 md:flex md:space-x-8">
                {navLinks.map((link) => {
                  const Icon = link.icon;
                  const isActive = location.pathname === link.path || location.pathname.startsWith(`${link.path}/`);
                  return (
                    <Link
                      key={link.path}
                      to={link.path}
                      className={`inline-flex items-center px-1 pt-1 border-b-2 text-sm font-semibold transition-all duration-300 ${
                        isActive
                          ? 'border-blue-500 text-white translate-y-[-1px]'
                          : 'border-transparent text-slate-400 hover:text-slate-200 hover:border-slate-500 hover:translate-y-[-1px]'
                      }`}
                    >
                      <Icon className={`w-4 h-4 mr-2 ${isActive ? 'text-blue-400' : 'text-slate-500'}`} />
                      {link.label}
                    </Link>
                  );
                })}
              </div>
            </div>

            {/* Right side actions */}
            <div className="hidden md:flex items-center space-x-6">
              <button className="relative p-2 rounded-xl text-slate-400 hover:text-white hover:bg-white/5 transition-all">
                <Bell className="w-5 h-5" />
                <span className="absolute top-2 right-2 block w-2.5 h-2.5 rounded-full bg-blue-500 ring-2 ring-[#0f172a]" />
              </button>

              <div className="flex items-center pl-6 border-l border-white/10 space-x-4">
                <div className="flex flex-col text-right">
                  <span className="text-sm font-bold text-white">{user?.fullName}</span>
                  <span className="text-[10px] text-blue-400 font-extrabold uppercase tracking-[0.1em]">{user?.role}</span>
                </div>
                <div className="w-10 h-10 rounded-xl bg-gradient-to-tr from-blue-500 to-indigo-600 flex items-center justify-center text-white font-bold shadow-md ring-1 ring-white/20 transform hover:scale-105 transition-transform cursor-pointer">
                  {user?.fullName?.charAt(0).toUpperCase()}
                </div>
                <button 
                  onClick={logout}
                  className="p-2 rounded-xl text-slate-500 hover:text-rose-400 hover:bg-rose-500/5 transition-all"
                  title="Logout"
                >
                  <LogOut className="w-5 h-5" />
                </button>
              </div>
            </div>

            {/* Mobile menu button */}
            <div className="flex items-center md:hidden">
              <button
                onClick={() => setIsMobileMenuOpen(!isMobileMenuOpen)}
                className="p-2 rounded-xl text-slate-400 hover:text-white hover:bg-white/5 focus:outline-none transition-all"
              >
                {isMobileMenuOpen ? <X className="w-6 h-6" /> : <Menu className="w-6 h-6" />}
              </button>
            </div>
          </div>
        </div>

        {/* Mobile menu */}
        <AnimatePresence>
          {isMobileMenuOpen && (
            <motion.div
              initial={{ opacity: 0, height: 0 }}
              animate={{ opacity: 1, height: 'auto' }}
              exit={{ opacity: 0, height: 0 }}
              className="md:hidden bg-[#0f172a]/95 backdrop-blur-2xl border-b border-white/10 overflow-hidden"
            >
              <div className="px-4 pt-2 pb-6 space-y-2">
                <div className="flex items-center space-x-4 mb-6 p-4 bg-white/5 rounded-2xl border border-white/5">
                  <div className="w-12 h-12 rounded-xl bg-gradient-to-tr from-blue-500 to-indigo-600 flex items-center justify-center text-white font-bold text-xl">
                    {user?.fullName?.charAt(0).toUpperCase()}
                  </div>
                  <div>
                    <div className="text-base font-bold text-white">{user?.fullName}</div>
                    <div className="text-xs text-blue-400 font-bold uppercase tracking-wider">{user?.role}</div>
                  </div>
                </div>
                
                {navLinks.map((link) => {
                  const Icon = link.icon;
                  const isActive = location.pathname === link.path || location.pathname.startsWith(`${link.path}/`);
                  return (
                    <Link
                      key={link.path}
                      to={link.path}
                      onClick={() => setIsMobileMenuOpen(false)}
                      className={`flex items-center px-4 py-3 rounded-xl text-base font-semibold transition-all ${
                        isActive
                          ? 'bg-blue-600/10 text-blue-400 border border-blue-500/20'
                          : 'text-slate-400 hover:bg-white/5 hover:text-white'
                      }`}
                    >
                      <Icon className="w-5 h-5 mr-3" />
                      {link.label}
                    </Link>
                  );
                })}
                <button
                  onClick={() => {
                    setIsMobileMenuOpen(false);
                    logout();
                  }}
                  className="flex items-center w-full text-left px-4 py-3 rounded-xl text-base font-semibold text-rose-400 hover:bg-rose-500/10 transition-all"
                >
                  <LogOut className="w-5 h-5 mr-3" />
                  Sign Out
                </button>
              </div>
            </motion.div>
          )}
        </AnimatePresence>
      </nav>

      {/* Main Content Area */}
      <main className="flex-1 w-full max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-10 relative z-10">
        <Outlet />
      </main>
    </div>
  );
};

export default DashboardLayout;
