import React, { useState } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { Menu, X, Bell } from 'lucide-react';
import { useAuth } from '../../context/AuthContext';
import { Link } from 'react-router-dom';

const Navbar: React.FC = () => {
  const [isMobileMenuOpen, setIsMobileMenuOpen] = useState(false);
  const { isAuthenticated, user, logout } = useAuth();

  const navLinks = [
    { name: 'Home', path: '/' },
    { name: 'Conferences', path: '/conferences' },
    { name: 'Schedule', path: '/schedule' },
    { name: 'About', path: '/about' },
  ];

  const authLinks = isAuthenticated ? [
    { name: 'Dashboard', path: '/dashboard' },
    { name: 'My Submissions', path: '/submissions' },
    ...(user?.role === 'REVIEWER' || user?.role === 'ADMIN' ? [{ name: 'Peer Reviews', path: '/reviews' }] : []),
  ] : [];

  return (
    <nav className="fixed top-0 left-0 w-full z-50 bg-[#0f172a]/75 backdrop-blur-xl border-b border-white/10">
      <div className="max-w-7xl mx-auto px-5 sm:px-8 py-4 flex justify-between items-center">
        {/* Logo */}
        <Link to="/" className="text-2xl font-extrabold tracking-tight">
          Conference
          <span className="bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent ml-1">
            Manager
          </span>
        </Link>

        {/* Desktop Links */}
        <div className="hidden md:flex items-center space-x-8 text-slate-300 font-medium">
          {navLinks.map((link) => (
            <Link key={link.name} to={link.path} className="hover:text-blue-400 transition-colors">
              {link.name}
            </Link>
          ))}
          {authLinks.map((link) => (
            <Link key={link.name} to={link.path} className="hover:text-blue-400 transition-colors">
              {link.name}
            </Link>
          ))}
        </div>

        {/* Right Actions */}
        <div className="flex items-center space-x-4">
          {!isAuthenticated ? (
            <div className="flex items-center space-x-4">
              <Link to="/login" className="text-white hover:text-blue-400 font-bold transition-all px-4 py-2">
                Login
              </Link>
              <Link to="/register" className="bg-blue-600 hover:bg-blue-500 text-white font-bold py-2.5 px-6 rounded-xl shadow-lg shadow-blue-500/20 transition-all transform hover:scale-105 active:scale-95">
                Register Now
              </Link>
            </div>
          ) : (
            <div className="flex items-center space-x-4">
              <div className="relative cursor-pointer group">
                <Bell size={20} className="text-slate-400 group-hover:text-blue-400 transition" />
                <span className="absolute -top-1 -right-1 bg-red-500 text-white text-[10px] font-bold w-4 h-4 flex items-center justify-center rounded-full">
                  3
                </span>
              </div>
              <div className="flex items-center gap-2 pl-2 border-l border-white/10">
                <div className="w-9 h-9 rounded-full bg-gradient-to-tr from-blue-500 to-purple-500 flex items-center justify-center text-white font-bold cursor-pointer shadow-md">
                  {user?.firstName?.charAt(0)}{user?.lastName?.charAt(0)}
                </div>
                <button onClick={logout} className="text-xs text-slate-500 hover:text-red-400 transition">
                  Logout
                </button>
              </div>
            </div>
          )}

          {/* Mobile Toggle */}
          <button
            className="md:hidden text-white focus:outline-none"
            onClick={() => setIsMobileMenuOpen(!isMobileMenuOpen)}
          >
            {isMobileMenuOpen ? <X size={28} /> : <Menu size={28} />}
          </button>
        </div>
      </div>

      {/* Mobile Menu */}
      <AnimatePresence>
        {isMobileMenuOpen && (
          <motion.div
            initial={{ opacity: 0, height: 0 }}
            animate={{ opacity: 1, height: 'auto' }}
            exit={{ opacity: 0, height: 0 }}
            className="md:hidden bg-[#0f172a] border-t border-white/10 overflow-hidden"
          >
            <div className="p-5 flex flex-col space-y-4 text-slate-300">
              {navLinks.map((link) => (
                <Link key={link.name} to={link.path} onClick={() => setIsMobileMenuOpen(false)}>
                  {link.name}
                </Link>
              ))}
              {authLinks.map((link) => (
                <Link key={link.name} to={link.path} onClick={() => setIsMobileMenuOpen(false)}>
                  {link.name}
                </Link>
              ))}
              {!isAuthenticated && (
                <div className="pt-4 flex flex-col space-y-3">
                  <Link to="/login" className="btn-primary text-center">Login</Link>
                </div>
              )}
            </div>
          </motion.div>
        )}
      </AnimatePresence>
    </nav>
  );
};

export default Navbar;
